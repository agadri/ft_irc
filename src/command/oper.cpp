/*
    Command: OPER
    Parameters: <name> <password>

    A normal user uses the OPER command to obtain operator privileges.
    The combination of <name> and <password> are REQUIRED to gain
    Operator privileges.  Upon success, the user will receive a MODE
    message (see section 3.1.5) indicating the new user modes.

    Numeric Replies:

            ERR_NEEDMOREPARAMS              RPL_YOUREOPER
            ERR_NOOPERHOST                  ERR_PASSWDMISMATCH

    Example:

    OPER foo bar                    ; Attempt to register as an operator
                                    using a username of "foo" and "bar"
                                    as the password.
*/

#include <command/command.h>

void IRC::Command::handleOPER(IServer *server, Client *client, CommandArguments *args) {
    if (args->size() < 2) {
        return client->send("461 OPER :Not enough parameters");
    }

    if (client->isOperator()) {
        return client->send("462 " + client->getNickname() + " :You are already an operator");
    }

    if (!server->verifyOperator(args->front(), args->at(1))) {
        return client->send("464 " + client->getNickname() + " :Password incorrect");
    }

    client->setOperator(true);

    client->send("381 " + client->getNickname() + " :You are now an IRC operator");
    client->addMode("o");
}