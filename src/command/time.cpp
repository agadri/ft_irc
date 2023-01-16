/*
    Command: TIME
    Parameters: [ <target> ]

    The time command is used to query local time from the specified
    server. If the <target> parameter is not given, the server receiving
    the command must reply to the query.

    Wildcards are allowed in the <target> parameter.

    Numeric Replies:

            ERR_NOSUCHSERVER              RPL_TIME

    Examples:
    TIME tolsun.oulu.fi             ; check the time on the server
                                    "tolson.oulu.fi"
*/

#include <command/command.h>

void IRC::Command::handleTIME(IServer *server, Client *client, CommandArguments *args) {
    if (args->size() == 0 || (args->size() == 1 && args->at(0) == server->getHostname())) {
        return client->send("391 " + client->getNickname() + " " + server->getHostname() + " :" + Logger::getTime());
    }

    return client->send("401 " + client->getNickname() + " " + args->at(0) + " :No such server");
}
