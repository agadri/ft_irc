/*
   Command: MOTD
   Parameters: [ <target> ]

   The MOTD command is used to get the "Message Of The Day" of the given
   server, or current server if <target> is omitted.

   Wildcards are allowed in the <target> parameter.

   Numeric Replies:
           RPL_MOTDSTART                   RPL_MOTD
           RPL_ENDOFMOTD                   ERR_NOMOTD
*/

#include <command/command.h>

void IRC::Command::handleMOTD(IServer *server, Client *client, CommandArguments *args) {
    (void)args;

    std::vector<std::string> motd = server->getMOTD();
    if (motd.size() == 0) {
        client->send("422 No MOTD");
        return ;
    }

    client->send("375 MOTD :Start of /MOTD command");
    for (std::vector<std::string>::iterator it = motd.begin(); it != motd.end(); ++it) {
        client->send("372 MOTD :" + *it);
    }
    client->send("376 MOTD :End of /MOTD command");
}