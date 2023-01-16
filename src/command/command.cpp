#include <command/command.h>

#include <string>
#include <pkg/smanip.h>

using namespace IRC::Command;

::ECommand IRC::Command::getCommand(std::string command) {
    command = Lib::Smanip::toUpper(command);

    if (command == "CAP") {
        return ::CAP;
    } else if (command == "JOIN") {
        return ::JOIN;
    } else if (command == "NICK") {
        return ::NICK;
    } else if (command == "USER") {
        return ::USER;
    } else if (command == "PASS") {
        return ::PASS;
    } else if (command == "PART") {
        return ::PART;
    } else if (command == "LUSERS") {
        return ::LUSERS;
    } else if (command == "MODE") {
        return ::MODE;
    } else if (command == "MOTD") {
        return ::MOTD;
    } else if (command == "WHOIS") {
        return ::WHOIS;
    } else if (command == "PING") {
        return ::PING;
    } else if (command == "PRIVMSG") {
        return ::PRIVMSG;
    } else if (command == "QUIT") {
        return ::QUIT;
    } else if (command == "NOTICE") {
        return ::NOTICE;
    } else if (command == "OPER") {
        return ::OPER;
    } else if (command == "INVITE") {
        return ::INVITE;
    } else if (command == "KICK") {
        return ::KICK;
    } else if (command == "LIST") {
        return ::LIST;
    } else if (command == "KILL") {
        return ::KILL;
    } else if (command == "NAMES") {
        return ::NAMES;
    } else if (command == "TIME") {
        return ::TIME;
    } else if (command == "TOPIC") {
        return ::TOPIC;
    }

    return ::UNDEFINED;
}

::Handler IRC::Command::getHandler(::ECommand command) {
    switch (command) {
        case ::CAP:
            return &::handleCAP;
        case ::JOIN:
            return &::handleJOIN;
        case ::NICK:
            return &::handleNICK;
        case ::USER:
            return &::handleUSER;
        case ::PASS:
            return &::handlePASS;
        case ::PART:
            return &::handlePART;
        case ::LUSERS:
            return &::handleLUSERS;
        case ::MOTD:
            return &::handleMOTD;
        case ::WHOIS:
            return &::handleWHOIS;
        case ::MODE:
            return &::handleMODE;
        case ::PING:
            return &::handlePING;
        case ::PRIVMSG:
            return &::handlePRIVMSG;
        case ::QUIT:
            return &::handleQUIT;
        case ::NOTICE:
            return &::handleNOTICE;
        case ::OPER:
            return &::handleOPER;
        case ::INVITE:
            return &::handleINVITE;
        case ::KICK:
            return &::handleKICK;
        case ::KILL:
            return &::handleKILL;
        case ::LIST:
            return &::handleLIST;
        case ::NAMES:
            return &::handleNAMES;
        case ::TIME:
            return &::handleTIME;
        case ::TOPIC:
            return &::handleTOPIC;
        default:
            return NULL;
    }
}