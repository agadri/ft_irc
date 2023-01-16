/*
    Command: USER
    Parameters: <user> <mode> <unused> <realname>

    The USER command is used at the beginning of connection to specify
    the username, hostname and realname of a new user.

    The <mode> parameter should be a numeric, and can be used to
    automatically set user modes when registering with the server.  This
    parameter is a bitmask, with only 2 bits having any signification: if
    the bit 2 is set, the user mode 'w' will be set and if the bit 3 is
    set, the user mode 'i' will be set.  (See Section 3.1.5 "User
    Modes").

    The <realname> may contain space characters.

    Numeric Replies:

            ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRED

    Example:

    USER guest 0 * :Ronnie Reagan   ; User registering themselves with a
                                    username of "guest" and real name
                                    "Ronnie Reagan".

    USER guest 8 * :Ronnie Reagan   ; User registering themselves with a
                                    username of "guest" and real name
                                    "Ronnie Reagan", and asking to be set
                                    invisible.
*/
#include <command/command.h>

void IRC::Command::handleUSER(IServer *server, Client *client, CommandArguments *args) {
    if (args->size() < 4) {
        return client->send("461 USER :Not enough parameters");
    }

    if (client->isRegistered()) {
        return client->send("462 USER :Unauthorized command (already registered)");
    }

    client->setUsername(args->at(0));

    std::string realname = "";
    for (unsigned int i = 3; i < args->size(); i++) {
        realname += args->at(i) + " ";
    }

    client->setRealname(realname);
    client->setRegistered(true);

    IRC::Command::handleHELLO(server, client, args);
    IRC::Command::handleMOTD(server, client, args);

    args->clear();
    IRC::Command::handleLUSERS(server, client, args);
}
