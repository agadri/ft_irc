#include <engine/server.h>
#include <engine/parser.h>
#include <client/client.h>
#include <pkg/logger.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include <iostream>
#include <cerrno>
#include <cstring>
#include <utility>
#include <vector>

using IRC::Engine::Server;
using IRC::Lib::Logger;

Server::Server() {}

Server::~Server() {
    /* Close every client */
    std::map<int, IClient *>::iterator it = _clients.begin();
    for (; it != _clients.end(); ++it) {
        Logger(Logger::INFO) << "Closing client " << it->first;
        delete it->second;
    }

    std::map<std::string, IChannel *>::iterator it2 = _channels.begin();
    for (; it2 != _channels.end(); ++it2) {
        Logger(Logger::INFO) << "Closing channel " << it2->first;
        delete it2->second;
    }

    /* Close the socket fd */
    if (_socketFd != -1) {
        close(_socketFd);
    }

    /* Cleanup epoll fd */
    if (_epoll_fd != -1) {
        close(_epoll_fd);
    }
}

int Server::run() {
    if (!isSetup()) {
        Logger(Logger::ERROR) << "Poll has not been setup";
        return 1;
    }

    Logger(Logger::INFO) << "Running server on port " << _port;
    if (_is_protected) {
        Logger(Logger::INFO) << "Using password: " << _password;
    }

    struct epoll_event events[MAX_POLLING];

    _alive = true;
    while (_alive) {
        int nfds = epoll_wait(_epoll_fd, events, MAX_POLLING, 100);
        if (nfds == -1) {
            Logger(Logger::ERROR) << "Failed to wait for epoll events: "
                << std::strerror(errno);
            return 1;
        }

        if (nfds == 0) {
            /* Collect waiting clients */
            std::map<int, IClient*>::iterator it = _clients.begin();
            for (it = _clients.begin(); it != _clients.end(); ++it) {
                if (!it->second->pendingPackets.empty()) {
                    setEpollState(it->first, EPOLLOUT);
                }
            }

            /* Collect dead channels */
            std::map<std::string, IChannel*>::iterator it2 = _channels.begin();
            for (it2 = _channels.begin(); it2 != _channels.end(); ++it2) {
                if (it2->second->getMemberCount() == 0) {
                    delete it2->second;
                    _channels.erase(it2);
                    break;
                }
            }

            continue;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == STDIN_FILENO) {
                handleStdin();
            } else if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP)) {
                handleLeave(events[i].data.fd);
            } else if (events[i].events & EPOLLIN) {
                handleRead(events[i].data.fd);
            } else if (events[i].events & EPOLLOUT) {
                handleWrite(events[i].data.fd);
            }
        }
    }

    return 0;
}

int Server::addFd(int targetFd) {
    // https://man7.org/linux/man-pages/man2/epoll_wait.2.html
    struct epoll_event epev = {};

    /* Set event type */
    epev.events = EPOLLIN;
    epev.data.fd = targetFd;

    /* Contact epoll_ctl to add the fd to the epoll instance */
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, targetFd, &epev) == -1) {
        Logger(Logger::ERROR) << "Failed to add " <<
            targetFd << " to epoll: " << std::strerror(errno);
        return 1;
    }

    return 0;
}

int Server::removeFd(int targetFd) {
    /* Contact epoll_ctl to remove the fd from the epoll instance */
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, targetFd, NULL) == -1) {
        Logger(Logger::ERROR) << "Failed to remove fd from epoll: "
            << std::strerror(errno);
        return 1;
    }

    return 0;
}

void Server::handleStdin() {
    std::string line;

    if (!std::getline(std::cin, line) || line == "quit" || line == "exit") {
        _alive = false;
    }
}

void Server::handleLeave(int fd) {
    Logger(Logger::INFO) << "Removing " << fd << " from epoll";

    /* Remove the fd from the epoll instance */
    removeFd(fd);

    /* Remove the client from the list of clients */
    std::map<int, IClient*>::iterator it = _clients.find(fd);
    if (it != _clients.end()) {
        Client* client = reinterpret_cast<Client *>(it->second);
        
        delete client;
        _clients.erase(it);

        Logger(Logger::INFO) << "Removed client " << fd;
    }
}

void Server::setEpollState(int targetFd, int state) {
    struct epoll_event event = {};

    event.events = state;
    event.data.fd = targetFd;

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, targetFd, &event) == -1) {
        Logger(Logger::ERROR) << "Failed to set epoll state: "
            << std::strerror(errno);

        return;
	}
}

void Server::handleAccept() {
    Logger(Logger::INFO) << "Received new connection";

    const int newFd = accept(_socketFd, NULL, NULL);
    if (newFd == -1) {
        Logger(Logger::ERROR) << "Failed to accept new connection: "
            << std::strerror(errno);
        return;
    }

    if (setNonBlocking(newFd) != 0) {
        return;
    }

    if (addFd(newFd) != 0) {
        Logger(Logger::ERROR) << "Failed to add new connection to epoll";
        return;
    }

    _clients.insert(std::pair<int, Client*>(
        newFd, 
        new Client(newFd, _is_protected, getHostname()))
    );

    setEpollState(newFd, EPOLLOUT);
}

IRC::Client *Server::findClient(int fd) {
    std::map<int, IClient*>::iterator it = _clients.find(fd);
    if (it == _clients.end()) {
        Logger(Logger::ERROR) << "Failed to find client with fd " << fd;
        return NULL;
    }

    IClient* client = it->second;
    if (client == NULL) {
        Logger(Logger::ERROR) << "Client with fd " << fd << " is NULL";
        return NULL;
    }

    return reinterpret_cast<Client *>(client);
}

void Server::handleRead(int fd) {
    if (fd == _socketFd) {
        return handleAccept();
    }

    Client *client = findClient(fd);
    if (client == NULL) {
        return;
    }

    char buffer[MAX_PACKET_SIZE] = {0};
    const int readSize = recv(fd, buffer, MAX_PACKET_SIZE, 0);
    if (readSize == -1) {
        Logger(Logger::ERROR) << "Failed to read from fd " <<
            fd << ": " << std::strerror(errno);
        return handleLeave(fd);
    }

    if (readSize == 0) {
        return handleLeave(fd);
    }

    Logger(Logger::DEBUG) << "Read " << readSize << " bytes from " << fd;
    buffer[readSize] = '\0';
    client->readBuffer += buffer;

    // if (it->second->readBuffer.size() > MAX_PACKET_SIZE) {
    //     Logger(Logger::ERROR) << "Client " << fd << " sent too much data";
    //     return handleLeave(fd);
    // }

    std::vector<Parser::CommandPacket> commands = Parser::parse(&(client->readBuffer));
    std::vector<Parser::CommandPacket>::iterator it = commands.begin();
    for (; it != commands.end(); it++) {
        handleCommandPacket(client, &(*it));
    }

    setEpollState(fd, EPOLLOUT);
}

void Server::handleCommandPacket(Client *client, Parser::CommandPacket *packet) {
    Command::Handler executor = Command::getHandler(packet->first);
    if (executor) {
        executor(reinterpret_cast<IServer*>(this), client, &(packet->second));
    }
}

void Server::handleWrite(int fd) {
    Logger(Logger::INFO) << "Write to fd " << fd;

    Client *client = findClient(fd);
    if (client == NULL) {
        return;
    }

    if (client->pendingPackets.empty()) {
        setEpollState(fd, EPOLLIN);
        return;
    }

    std::string packet = "";
    while (!client->pendingPackets.empty()) {
        packet += client->pendingPackets.front() + "\r\n";
        client->pendingPackets.erase(client->pendingPackets.begin());
    }

    if (packet.size() > 0) {
        const int writeSize = send(fd, packet.c_str(), packet.size(), 0);
        if (writeSize == -1) {
            Logger(Logger::ERROR) << "Failed to write to fd " <<
                fd << ": " << std::strerror(errno);
            return handleLeave(fd);
        }

        Logger(Logger::DEBUG) << "Wrote " << writeSize << " bytes to " << fd;
        Logger(Logger::DEBUG) << "Sent: " << packet.c_str();
    }

    setEpollState(fd, EPOLLIN);
}

int Server::setNonBlocking(int targetFd) const {
    if (fcntl(targetFd, F_SETFL, O_NONBLOCK) == -1) {
        Logger(Logger::ERROR) << "Failed to set socket to non-blocking: "
            << std::strerror(errno);
        return 1;
    }

    return 0;
}

int Server::setup() {
    /* Create socket */
    _socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socketFd == -1) {
        Logger(Logger::ERROR) << "Failed to create socket: "
            << std::strerror(errno);
        return 1;
    }

    /* Set socket to non-blocking */
    if (setNonBlocking(_socketFd) != 0) {
        return 1;
    }

    /* Set reuse address */
    int _reuseAddr = 1;
    if (setsockopt(_socketFd, SOL_SOCKET,
        SO_REUSEADDR, &_reuseAddr, sizeof(_reuseAddr)) == -1) {
        Logger(Logger::ERROR) << "Failed to set socket to reuse address: "
            << std::strerror(errno);
        return 1;
    }

    /* Setup socket address */
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* Bind socket */
    if (bind(_socketFd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        Logger(Logger::ERROR) << "Failed to bind socket: "
            << std::strerror(errno);
        return 1;
    }

    /* Listen on socket */
    if (listen(_socketFd, SOMAXCONN) != 0) {
        Logger(Logger::ERROR) << "Failed to listen on socket: "
            << std::strerror(errno);
        return 1;
    }

    /* Check if epoll is already setup */
    if (_epoll_fd != -1) {
        Logger(Logger::ERROR) << "Poll already setup";
        return 0;
    }

    /* Create epoll file descriptor */
    _epoll_fd = epoll_create1(0);
    if (_epoll_fd == -1) {
        Logger(Logger::ERROR) << "Failed to create epoll instance: "
            << std::strerror(errno);
        return 1;
    }

    /* Add stdin to epoll */
    if (addFd(STDIN_FILENO) != 0) {
        return 1;
    }

    /* Register server socket */
    if (addFd(_socketFd) != 0) {
        return 1;
    }

    return 0;
}
