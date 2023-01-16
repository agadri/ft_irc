#include <command/command.h>

void IRC::Command::handleHELLO(IServer *server, Client *client, CommandArguments *args) {
    (void)args;

    client->send("001 " + client->getNickname() + " :Welcome to the Internet Relay Network " + client->getNickname() + "!" + client->getUsername() + "@" + server->getHostname());
    client->send("002 " + client->getNickname() + " :Your host is " + server->getHostname() + ", running version " + server->getVersion());
    client->send("003 " + client->getNickname() + " :This server was created " + server->getCreationDate());
    client->send("004 " + server->getServername() + " " + server->getVersion() + " " + server->getAvailableUserModes() + " " + server->getAvailableChannelModes());
}