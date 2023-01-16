#pragma once

#include <client/iclient.h>

#include <string>
#include <vector>

class IChannel {
 protected:
    std::string _name;
    std::string _topic;
    std::string _key;

    std::vector<IClient *> _banneds;
    std::vector<IClient *> _members;
    std::vector<IClient *> _operators;

 public:
    explicit IChannel(const std::string &name) {
        _name = name;
    }
    virtual ~IChannel() {};

    std::string getName() const {
        return _name;
    }
    std::string getTopic() const {
        return _topic;
    }
    std::string getKey() const {
        return _key;
    }

    void setName(const std::string &name) {
        _name = name;
    }
    void setTopic(const std::string &topic) {
        _topic = topic;
    }
    void setKey(const std::string &key) {
        _key = key;
    }

    bool isKeyProtected() const {
        return !_key.empty();
    }

    bool isBanned(IClient *client) const {
        std::vector<IClient *>::const_iterator it;

        for (it = _banneds.begin(); it != _banneds.end(); ++it) {
            if (*it == client) {
                return true;
            }
        }

        return false;
    }
    bool isMember(IClient *client) const {
        std::vector<IClient *>::const_iterator it;

        for (it = _members.begin(); it != _members.end(); ++it) {
            if (*it == client) {
                return true;
            }
        }

        return false;
    }

    void addMember(IClient *client) {
        if (!isMember(client)) {
            _members.push_back(client);
        }
    }

    void removeMember(IClient *client) {
        std::vector<IClient *>::iterator it;

        for (it = _members.begin(); it != _members.end(); ++it) {
            if (*it == client) {
                _members.erase(it);
                break;
            }
        }
    }

    void send(std::string message) {
        return send(message, NULL);
    }

    void send(std::string message, IClient *except) {
        std::vector<IClient *>::iterator it;

        for (it = _members.begin(); it != _members.end(); ++it) {
            if (*it != except) {
                (*it)->send(message);
            }
        }
    }

    std::string getNames() const {
        std::string names;
        std::vector<IClient *>::const_iterator it;

        for (it = _members.begin(); it != _members.end(); ++it) {
            names += (*it)->getUsername() + " ";
        }

        return names;
    }

    bool isOperator(IClient *client) const {
        std::vector<IClient *>::const_iterator it;

        for (it = _operators.begin(); it != _operators.end(); ++it) {
            if (*it == client) {
                return true;
            }
        }

        return false;
    }

    void setOperator(IClient *client, bool doOP) {
        if (doOP && !isOperator(client)) {
            _operators.push_back(client);
        } else if (!doOP && isOperator(client)) {
            std::vector<IClient *>::iterator it;

            for (it = _operators.begin(); it != _operators.end(); ++it) {
                if (*it == client) {
                    _operators.erase(it);
                    break;
                }
            }
        }
    }

    size_t getMemberCount() const {
        return _members.size();
    }
};
