// https://ircv3.net/specs/extensions/capability-negotiation.html#the-cap-command

#include <command/command.h>
#include <pkg/logger.h>

#include <iostream>

using IRC::Lib::Logger;

void IRC::Command::handleCAP(IServer *server, Client *client, CommandArguments *args) {
    (void)server;

    if (args->size() == 0) {
        Logger(Logger::DEBUG) << "CAP: No arguments\n";
        return;
    }
    
    if (args->front() == "END") {
        return ;
    }

    if (args->front() == "LS") {
        client->send("CAP * LS :");
    }
}