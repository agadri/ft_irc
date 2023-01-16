#pragma once

#include <unistd.h>

#include <string>
#include <vector>

class IClient {
 protected:
    int _fd;
    
    std::string _nick;
    std::string _username;
    std::string _realname;
    std::string _hostname;
    
    bool _operator;
 public:
    std::vector<std::string> pendingPackets;

 public:
    virtual ~IClient() {
        if (_fd != -1) {
            close(_fd);
        }
    }

    void send(std::string message) {
        pendingPackets.push_back(message);
    }

    std::string getNickname() const {
        return _nick;
    }

    std::string getMessageName() const {
        return _nick + "!" + _username + "@" + _hostname;
    }

    void setNickname(const std::string &nickname) {
        _nick = nickname;
    }

    bool isOperator() const {
        return _operator;
    }
    void setOperator(bool op) {
        _operator = op;
    }
    int getFd() const {
        return _fd;
    }

    void setUsername(const std::string &username) {
        _username = username;
    }
    void setRealname(const std::string &realname) {
        _realname = realname;
    }
    std::string getUsername() const {
        return _username;
    }
    std::string getRealname() const {
        return _realname;
    }
};
