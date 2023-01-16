#pragma once

#include <client/client.h>
#include <engine/iserver.h>

#include <string>
namespace IRC {

namespace Command {

enum ECommand {
    AWAY,
    CAP,
    INVITE,
    JOIN,
    KICK,
    LUSERS,
    MODE,
    MOTD,
    NICK,
    NOTICE,
    OPER,
    PART,
    PASS,
    PING,
    PRIVMSG,
    QUIT,
    UNDEFINED,
    USER,
    WHOIS,
    KILL,
    LIST,
    NAMES,
    TIME,
    TOPIC,
};

typedef std::vector<std::string> CommandArguments;
typedef void (*Handler)(IServer *server, Client *client, CommandArguments *args);

ECommand getCommand(std::string command);
Handler getHandler(ECommand command);

void handleAWAY(IServer *server, Client *client, CommandArguments *args);
void handleCAP(IServer *server, Client *client, CommandArguments *args);
void handleHELLO(IServer *server, Client *client, CommandArguments *args);
void handleINVITE(IServer *server, Client *client, CommandArguments *args);
void handleJOIN(IServer *server, Client *client, CommandArguments *args);
void handleKICK(IServer *server, Client *client, CommandArguments *args);
void handleKILL(IServer *server, Client *client, CommandArguments *args);
void handleLUSERS(IServer *server, Client *client, CommandArguments *args);
void handleMODE(IServer *server, Client *client, CommandArguments *args);
void handleMOTD(IServer *server, Client *client, CommandArguments *args);
void handleNICK(IServer *server, Client *client, CommandArguments *args);
void handleNOTICE(IServer *server, Client *client, CommandArguments *args);
void handleOPER(IServer *server, Client *client, CommandArguments *args);
void handlePART(IServer *server, Client *client, CommandArguments *args);
void handlePASS(IServer *server, Client *client, CommandArguments *args);
void handlePING(IServer *server, Client *client, CommandArguments *args);
void handlePRIVMSG(IServer *server, Client *client, CommandArguments *args);
void handleQUIT(IServer *server, Client *client, CommandArguments *args);
void handleUSER(IServer *server, Client *client, CommandArguments *args);
void handleWHOIS(IServer *server, Client *client, CommandArguments *args);
void handleLIST(IServer *server, Client *client, CommandArguments *args);
void handleNAMES(IServer *server, Client *client, CommandArguments *args);
void handleTIME(IServer *server, Client *client, CommandArguments *args);
void handleTOPIC(IServer *server, Client *client, CommandArguments *args);

}  // namespace Command
}  // namespace IRC