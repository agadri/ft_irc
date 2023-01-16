/*
    Command: TOPIC
    Parameters: <channel> [ <topic> ]

    The TOPIC command is used to change or view the topic of a channel.
    The topic for channel <channel> is returned if there is no <topic>
    given.  If the <topic> parameter is present, the topic for that
    channel will be changed, if this action is allowed for the user
    requesting it.  If the <topic> parameter is an empty string, the
    topic for that channel will be removed.

    Numeric Replies:

            ERR_NEEDMOREPARAMS              ERR_NOTONCHANNEL
            RPL_NOTOPIC                     RPL_TOPIC
            ERR_CHANOPRIVSNEEDED            ERR_NOCHANMODES

    Examples:

    :WiZ!jto@tolsun.oulu.fi TOPIC #test :New topic ; User Wiz setting the
                                    topic.

    TOPIC #test :another topic      ; Command to set the topic on #test
                                    to "another topic".

    TOPIC #test :                   ; Command to clear the topic on
                                    #test.

    TOPIC #test                     ; Command to check the topic for
                                    #test.
*/

#include <command/command.h>

void IRC::Command::handleTOPIC(IServer *server, Client *client, CommandArguments *args) {
    if (args->size() < 1) {
        return client->send("461 " + client->getNickname() + " TOPIC :Not enough parameters");
    }

    if (args->size() > 3) {
        return client->send("461 " + client->getNickname() + " TOPIC :Too many parameters");
    }

    IChannel *channel = server->getChannel(args->at(0));
    if (channel == NULL) {
        return client->send("403 " + client->getNickname() + " " + args->at(0) + " :No such channel");
    }

    if (!channel->isMember(client)) {
        return client->send("442 " + client->getNickname() + " " + channel->getName() + " :You're not on that channel");
    }

    if (args->size() == 1) {
        if (channel->getTopic().empty()) {
            return client->send("331 " + client->getNickname() + " " + channel->getName() + " :No topic is set");
        }

        return client->send("332 " + client->getNickname() + " " + channel->getName() + " :" + channel->getTopic());
    }

    if (!channel->isOperator(client)) {
        return client->send("482 " + client->getNickname() + " " + channel->getName() + " :You're not channel operator");
    }

    channel->setTopic(args->at(1));
    return channel->send(":" + client->getMessageName() + " TOPIC " + args->at(0) + " " + channel->getTopic());
}