#pragma once

#include <pkg/logger.h>
#include <string>

class IBot {
 public:
    IBot() {}

    std::string eval(std::string cmd, std::string target) const {
        std::string resp;

        if (endsWith(cmd, ":!ping")) {
            resp = sendPong();
        } else if (endsWith(cmd, ":!time")) {
            resp = sendTime();
        } else if (endsWith(cmd, ":!who")) {
            resp = sendWho();
        } else if (endsWith(cmd, ":!help")) {
            resp = sendHelp();
        }

        if (resp.empty()) {
            return "";
        }

        return ":IRCBOT PRIVMSG " + target + " :" + resp;;
    }

    bool endsWith(std::string const &fullString, std::string const &ending) const {
        if (fullString.length() >= ending.length()) {
            if (fullString.compare(fullString.length() - ending.length(), ending.length(), ending) == 0) {
                return true;
            }
        } 
        return false;
    }

    std::string sendPong() const {
        return "PONG!";
    }

    std::string sendTime() const {
        return "Current time: " + IRC::Lib::Logger::getTime();
    }

    std::string sendHelp() const {
        return "Available commands: !ping, !time, !who, !help";
    }

    std::string sendWho() const {
        return "I'm a bot!";
    }
};