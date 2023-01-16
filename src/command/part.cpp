/*
    Command: PART
    Parameters: <channel> *( "," <channel> ) [ <Part Message> ]

    The PART command causes the user sending the message to be removed
    from the list of active members for all given channels listed in the
    parameter string.  If a "Part Message" is given, this will be sent
    instead of the default message, the nickname.  This request is always
    granted by the server.

    Servers MUST be able to parse arguments in the form of a list of
    target, but SHOULD NOT use lists when sending PART messages to
    clients.

    Numeric Replies:

            ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
            ERR_NOTONCHANNEL

    Examples:

    PART #twilight_zone             ; Command to leave channel
                                    "#twilight_zone"

    PART #oz-ops,&group5            ; Command to leave both channels
                                    "&group5" and "#oz-ops".

    :WiZ!jto@tolsun.oulu.fi PART #playzone :I lost
                                    ; User WiZ leaving channel
                                    "#playzone" with the message "I
                                    lost".
*/

#include <command/command.h>
#include <pkg/smanip.h>

void IRC::Command::handlePART(IServer *server, Client *client, CommandArguments *args) {
    if (args->size() < 1) {
        return client->send("461 PART :Not enough parameters");
    }

    std::vector<std::string> channels = Lib::Smanip::split(args->at(0), ",");
    std::string message = args->size() > 1 ? args->at(1) : client->getNickname();

    std::vector<std::string>::iterator it = channels.begin();
    for (; it != channels.end(); ++it) {
        if ((*it)[0] != '#' && (*it)[0] != '&') {
            return client->send("403 " + client->getNickname() + " " + *it + " :No such channel");
        }

        if (client->getChannels()->find(*it) == client->getChannels()->end()) {
            return client->send("442 " + client->getNickname() + " " + *it + " :You're not on that channel");
        }

        IChannel *channel = server->getChannel(*it);

        if (message.at(0) == ':') {
            message = message.substr(1);
        }
        channel->send(":" + client->getMessageName() + " PART " + *it + " :" + message);
        channel->removeMember(client);
        client->removeChannel(channel);
    }
}