/*
    Command: NICK
    Parameters: <nickname>

    NICK command is used to give user a nickname or change the existing
    one.

    Numeric Replies:

        ERR_NONICKNAMEGIVEN             ERR_ERRONEUSNICKNAME
        ERR_NICKNAMEINUSE               ERR_NICKCOLLISION
        ERR_UNAVAILRESOURCE             ERR_RESTRICTED

    Examples:

    NICK Wiz                ; Introducing new nick "Wiz" if session is
                        still unregistered, or user changing his
                        nickname to "Wiz"

    :WiZ!jto@tolsun.oulu.fi NICK Kilroy
                        ; Server telling that WiZ changed his
                        nickname to Kilroy.
*/
#include <command/command.h>

void IRC::Command::handleNICK(IServer *server, Client *client, CommandArguments *args) {
    if (args->size() < 1) {
        return client->send("431 NICK :No nickname given");
    }

    if (!client->isAuthenticated()) {
        return client->send("464 NICK :You must authenticate first");
    }

    if (args->size() > 1 || args->front().length() > 9) {
        return client->send("432 NICK :Erroneous nickname");
    }

    if (server->getClient(args->front()) != NULL) {
        return client->send("433 NICK :Nickname is already in use");
    }

    client->send(":" + client->getMessageName() + " NICK " + args->front());
    client->setNickname(args->front());
}