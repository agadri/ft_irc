#pragma once

#include <client/client.h>
#include <engine/parser.h>
#include <engine/iserver.h>

#include <string>
#include <map>

#define MAX_POLLING 64
#define MAX_PACKET_SIZE 512

namespace IRC {
namespace Engine {
class Server : public IServer {
 public:
    Server();
    ~Server();

    int setup();
    int run();

 private:
    void handleStdin();
    void handleLeave(int fd);
    void handleRead(int fd);
    void handleWrite(int fd);
    void handleAccept();
    void handleCommandPacket(Client *client, Parser::CommandPacket *packet);

    int addFd(int targetFd);
    int removeFd(int targetFd);
    int setNonBlocking(int targetFd) const;

    void setEpollState(int targetFd, int state);

    Client *findClient(int fd);

    bool isSetup() const { return _socketFd != -1; }
};
}  // namespace Engine
}  // namespace IRC
