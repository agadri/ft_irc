/*
    Command: PASS
    Parameters: <password>

    The PASS command is used to set a 'connection password'.  The
    optional password can and MUST be set before any attempt to register
    the connection is made.  Currently this requires that user send a
    PASS command before sending the NICK/USER combination.

    Numeric Replies:

            ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRED

    Example:

            PASS secretpasswordhere
*/

#include <command/command.h>

void IRC::Command::handlePASS(IServer *server, Client *client, CommandArguments *args) {
    if (server->isProtected()) {
        if (args->size() == 0) {
            return client->send("461 PASS :Not enough parameters");
        }

        if (!server->verifyPassword(args->front())) {
            return client->send("464 PASS :Password incorrect");
        }
    }

    if (client->isAuthenticated()) {
        return client->send("462 PASS :Unauthorized command (already authenticated)");
    }

    client->send("464 PASS :Password accepted");
    client->setAuthenticated(true);
}