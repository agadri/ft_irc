/*
    Command: NAMES
    Parameters: [ <channel> *( "," <channel> ) [ <target> ] ]

    By using the NAMES command, a user can list all nicknames that are
    visible to him. For more details on what is visible and what is not,
    see "Internet Relay Chat: Channel Management" [IRC-CHAN].  The
    <channel> parameter specifies which channel(s) to return information
    about.  There is no error reply for bad channel names.

    If no <channel> parameter is given, a list of all channels and their
    occupants is returned.  At the end of this list, a list of users who
    are visible but either not on any channel or not on a visible channel
    are listed as being on `channel' "*".

    If the <target> parameter is specified, the request is forwarded to
    that server which will generate the reply.

    Wildcards are allowed in the <target> parameter.

    Numerics:

            ERR_TOOMANYMATCHES              ERR_NOSUCHSERVER
            RPL_NAMREPLY                    RPL_ENDOFNAMES

    Examples:

    NAMES #twilight_zone,#42        ; Command to list visible users on
                                    #twilight_zone and #42

    NAMES                           ; Command to list all visible
                                    channels and users
*/

#include <command/command.h>
#include <pkg/smanip.h>

void IRC::Command::handleNAMES(IServer *server, Client *client, CommandArguments *args) {
    if (args->size() > 1) {
        return client->send("461 " + client->getNickname() + " NAMES :Too many parameters");
    }

    if (args->size() == 0) {
        std::map<std::string, IChannel *> *channels = client->getChannels();
        std::map<std::string, IChannel *>::const_iterator it;
        for (it = channels->begin(); it != channels->end(); ++it) {
            IChannel *channel = it->second;
            client->send("353 " + client->getNickname() + " = " + channel->getName() + " :" + channel->getNames());
        }
    } else {
        std::vector<std::string> channels = Lib::Smanip::split(args->at(0), ",");

        std::vector<std::string>::const_iterator it;
        for (it = channels.begin(); it != channels.end(); ++it) {
            IChannel *channel = server->getChannel(*it);
            if (channel == NULL) {
                client->send("403 " + client->getNickname() + " " + *it + " :No such channel");
                continue;
            }

            client->send("353 " + client->getNickname() + " = " + channel->getName() + " :" + channel->getNames());
        }
    }

    client->send("366 " + client->getNickname() + " * :End of /NAMES list");
}