#pragma once

#include <pkg/logger.h>
#include <engine/ichannel.h>
#include <engine/ibot.h>

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

using IRC::Lib::Logger;

class IServer {
 public:
    IBot bot;

 protected:
    std::string _password;
    std::string _motd;
    std::string _creationDate;

    bool _is_protected;

    int _port;
    int _epoll_fd;
    int _socketFd;
    bool _alive;

    std::map<int, IClient*> _clients;
    std::map<std::string, IChannel*> _channels;

 public:
    IServer() {
        _password = "";
        _creationDate = Logger::getTime();

        _is_protected = false;

        _port = 6697;
        _epoll_fd = -1;
        _socketFd = -1;
        _alive = false;
    }

    void setPort(const int &port) {
        if (port <= 0 || port > 65535) {
            throw std::invalid_argument("Invalid port number");
        }

        _port = port;
    }
    void setPassword(const std::string &password) {
        if (_password.length() > 32) {
            throw std::invalid_argument("Password must be less than 32 characters");
        }

        _password = password;
        _is_protected = true;
    }

    bool verifyPassword(const std::string &password) const {
        return password == _password;
    }
    bool isProtected() const {
        return _is_protected;
    }

    IClient *getClient(const std::string &nickname) {
        std::map<int, IClient*>::iterator it = _clients.begin();
        for (; it != _clients.end(); ++it) {
            if (it->second->getNickname() == nickname) {
                return it->second;
            }
        }

        return NULL;
    }

    std::vector<std::string> getMOTD() const {
        std::vector<std::string> motd;

        std::ifstream file("motd.txt");
        if (!file.is_open()) {
            return motd;
        }

        std::string line;
        while (std::getline(file, line)) {
            motd.push_back(line);
        }

        return motd;
    }

    std::string getHostname() const {
        return "localhost";
    }

    std::string getVersion() const {
        return "1.0.0";
    }

    std::string getCreationDate() const {
        return _creationDate;
    }

    std::string getServername() const {
        return "ft_irc";
    }

    std::string getAvailableUserModes() const {
        /*
            o = operator
        */
        return "o";
    }

    std::string getAvailableChannelModes() const {
        /*
            t = topic settable by channel operator only
        */
        return "t";
    }

    bool verifyOperator(const std::string &login, const std::string &password) const {
        return login == "admin" && password == "admin";
    }

    IChannel *getChannel(std::string name) {
        std::map<std::string, IChannel*>::iterator it = _channels.find(name);
        if (it == _channels.end()) {
            return NULL;
        }

        return it->second;
    }

    std::map<std::string, IChannel *> *getChannels() {
        return &_channels;
    }

    std::map<int, IClient *> *getClients() {
        return &_clients;
    }

    IChannel *createChannel(std::string name) {
        IChannel *channel = getChannel(name);
        if (channel != NULL) {
            return channel;
        }

        channel = new IChannel(name);
        _channels[name] = channel;

        return channel;
    }

    void removeClient(IClient *client) {
        if (client == NULL) {
            return;
        }

        _clients.erase(client->getFd());
        delete client;
    }
};
