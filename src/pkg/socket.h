#pragma once

namespace IRC {
namespace Lib {
namespace Socket {

static void setNonBlocking(int targetFd);
static void openSocket(int port);
static void closeSocket();
static void bindSocket();
static void listenSocket();

}  // namespace Socket
}  // namespace Lib
}  // namespace IRC
