#pragma once

#include <client/iclient.h>
#include <engine/ichannel.h>

#include <string>
#include <vector>
#include <map>

namespace IRC {
class Client: public IClient {
 private:
    bool _authenticated;
    bool _registered;

    std::string _modes;
    std::map<std::string, IChannel *> _channels;

 public:
    std::string readBuffer;

 public:
    explicit Client(int fd, bool serverIsProtected, std::string hostname);
    virtual ~Client() {};

    std::map<std::string, IChannel *> *getChannels();
    void addChannel(IChannel *channel);
    void removeChannel(IChannel *channel);

    void setAuthenticated(bool authenticated) {
        _authenticated = authenticated;
    }

    void setRegistered(bool registered);

    bool isAuthenticated() const {
        return _authenticated;
    }
    bool isRegistered() const {
        return _registered;
    }
    std::string getUsername() const {
        return _username;
    }
    std::string getModes() const {
        return _modes;
    }
    void addMode(const std::string &mode) {
        if (_modes.find(mode) == std::string::npos) {
            _modes += mode;
        }
    }
    void removeMode(const std::string &mode) {
        size_t pos = _modes.find(mode);
        if (pos != std::string::npos) {
            _modes.erase(pos, 1);
        }
    }
};
}  // namespace IRC
