/*
    Command: LIST
    Parameters: [ <channel> *( "," <channel> ) [ <target> ] ]

    The list command is used to list channels and their topics.  If the
    <channel> parameter is used, only the status of that channel is
    displayed.

    If the <target> parameter is specified, the request is forwarded to
    that server which will generate the reply.

    Wildcards are allowed in the <target> parameter.

    Numeric Replies:

            ERR_TOOMANYMATCHES              ERR_NOSUCHSERVER
            RPL_LIST                        RPL_LISTEND

    Examples:

    LIST                            ; Command to list all channels.

    LIST #twilight_zone,#42         ; Command to list channels
                                    #twilight_zone and #42
*/

#include <command/command.h>
#include <pkg/smanip.h>

void IRC::Command::handleLIST(IServer *server, Client *client, CommandArguments *args) {
    if (args->size() > 2) {
        return client->send("461 LIST :Too many parameters");
    }

    if (args->size() == 0) {
        std::map<std::string, IChannel *> *channels = server->getChannels();
        std::map<std::string, IChannel *>::const_iterator it;
        for (it = channels->begin(); it != channels->end(); ++it) {
            IChannel *channel = it->second;
            client->send("322 " + client->getNickname() + " " + channel->getName() + " " + channel->getTopic());
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
            client->send("322 " + client->getNickname() + " " + channel->getName() + " " + channel->getTopic());
        }
    }
}