#include <client/client.h>

#include <map>
#include <string>

using IRC::Client;

Client::Client(int fd, bool serverIsProtected, std::string hostname) {
    _fd = fd;
    _authenticated = !serverIsProtected;
    _registered = false;

    readBuffer = "";
    pendingPackets = std::vector<std::string>();
    _nick = "";
    _operator = false;
    _hostname = hostname;
}

void Client::setRegistered(bool registered) {
    _registered = registered;
}

std::map<std::string, IChannel *> *Client::getChannels() {
    return &_channels;
}

void Client::addChannel(IChannel *channel) {
    if (_channels.find(channel->getName()) == _channels.end()) {
        _channels[channel->getName()] = channel;
    }
}

void Client::removeChannel(IChannel *channel) {
    if (_channels.find(channel->getName()) != _channels.end()) {
        _channels.erase(channel->getName());
    }
}