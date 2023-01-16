/*
    Command: KICK
    Parameters: <channel> *( "," <channel> ) <user> *( "," <user> )
                [<comment>]

    The KICK command can be used to request the forced removal of a user
    from a channel.  It causes the <user> to PART from the <channel> by
    force.  For the message to be syntactically correct, there MUST be
    either one channel parameter and multiple user parameter, or as many
    channel parameters as there are user parameters.  If a "comment" is
    given, this will be sent instead of the default message, the nickname
    of the user issuing the KICK.

    The server MUST NOT send KICK messages with multiple channels or
    users to clients.  This is necessarily to maintain backward
    compatibility with old client software.

    Numeric Replies:

            ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
            ERR_BADCHANMASK                 ERR_CHANOPRIVSNEEDED
            ERR_USERNOTINCHANNEL            ERR_NOTONCHANNEL
    Examples:

    KICK &Melbourne Matthew         ; Command to kick Matthew from
                                    &Melbourne

    KICK #Finnish John :Speaking English
                                    ; Command to kick John from #Finnish
                                    using "Speaking English" as the
                                    reason (comment).

    :WiZ!jto@tolsun.oulu.fi KICK #Finnish John
                                    ; KICK message on channel #Finnish
                                    from WiZ to remove John from channel
*/
#include <command/command.h>

void IRC::Command::handleKICK(IServer *server, Client *client, CommandArguments *args) {
    if (args->size() < 2) {
        return client->send("461 " + client->getNickname() + " KICK :Not enough parameters");
    }

    if (args->size() > 3) {
        return client->send("461 " + client->getNickname() + " KICK :Too many parameters");
    }

    std::string channel_name = args->at(0);

    if (channel_name[0] != '#' && channel_name[0] != '&') {
        return client->send("403 " + client->getNickname() + " " + channel_name + " :No such channel");
    }

    if (client->getChannels()->find(channel_name) == client->getChannels()->end()) {
        return client->send("442 " + client->getNickname() + " " + channel_name + " :You're not on that channel");
    }

    IChannel *channel = server->getChannel(channel_name);
    if (!channel->isOperator(client)) {
        return client->send("482 " + client->getNickname() + " " + channel->getName() + " :You're not channel operator");
    }

    std::string user = args->at(1);
    std::string comment = args->size() == 3 ? args->at(2) : "";

    Client *target = reinterpret_cast<Client *>(server->getClient(user));

    if (target == NULL) {
        return client->send("401 " + client->getNickname() + " " + user + " :No such nick/channel");
    }

    if (!channel->isMember(target)) {
        return client->send("441 " + client->getNickname() + " " + target->getNickname() + " " + channel->getName() + " :They aren't on that channel");
    }

    if (comment.size() > 0 && comment[0] != ':') {
        comment = ":" + comment;
    }
    channel->send(":" + client->getMessageName() + " KICK " + channel->getName() + " " + target->getNickname() + comment);
    channel->removeMember(target);
    target->removeChannel(channel);
}