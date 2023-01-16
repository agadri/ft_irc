/*
    Command: JOIN
    Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] )
                / "0"

    The JOIN command is used by a user to request to start listening to
    the specific channel.  Servers MUST be able to parse arguments in the
    form of a list of target, but SHOULD NOT use lists when sending JOIN
    messages to clients.

    Once a user has joined a channel, he receives information about
    all commands his server receives affecting the channel.  This
    includes JOIN, MODE, KICK, PART, QUIT and of course PRIVMSG/NOTICE.
    This allows channel members to keep track of the other channel
    members, as well as channel modes.

    If a JOIN is successful, the user receives a JOIN message as
    confirmation and is then sent the channel's topic (using RPL_TOPIC) and
    the list of users who are on the channel (using RPL_NAMREPLY), which
    MUST include the user joining.

    Note that this message accepts a special argument ("0"), which is
    a special request to leave all channels the user is currently a member
    of.  The server will process this message as if the user had sent
    a PART command (See Section 3.2.2) for each channel he is a member
    of.

    Numeric Replies:

            ERR_NEEDMOREPARAMS              ERR_BANNEDFROMCHAN
            ERR_INVITEONLYCHAN              ERR_BADCHANNELKEY
            ERR_CHANNELISFULL               ERR_BADCHANMASK
            ERR_NOSUCHCHANNEL               ERR_TOOMANYCHANNELS
            ERR_TOOMANYTARGETS              ERR_UNAVAILRESOURCE
            RPL_TOPIC

    Examples:

    JOIN #foobar                    ; Command to join channel #foobar.

    JOIN &foo fubar                 ; Command to join channel &foo using
                                    key "fubar".

    JOIN #foo,&bar fubar            ; Command to join channel #foo using
                                    key "fubar" and &bar using no key.

    JOIN #foo,#bar fubar,foobar     ; Command to join channel #foo using
                                    key "fubar", and channel #bar using
                                    key "foobar".

    JOIN #foo,#bar                  ; Command to join channels #foo and
                                    #bar.

    JOIN 0                          ; Leave all currently joined
                                    channels.

    :WiZ!jto@tolsun.oulu.fi JOIN #Twilight_zone ; JOIN message from WiZ
                                    on channel #Twilight_zone
*/

#include <command/command.h>
#include <pkg/smanip.h>

void IRC::Command::handleJOIN(IServer *server, Client *client, CommandArguments *args) {
    if (args->size() == 1 && args->at(0) == ":") {
        return;
    }

    if (args->size() == 0) {
        return client->send("461 " + client->getNickname() + " JOIN");
    }

    if (args->size() > 2) {
        return client->send("461 " + client->getNickname() + " JOIN :Too many parameters");
    }

    // Leave all channels
    if (args->at(0) == "0") {
        std::string channels_joined = "";
        std::map<std::string, IChannel *> *channels = client->getChannels();

        std::map<std::string, IChannel *>::iterator it = channels->begin();
        for (; it != channels->end(); ++it) {
            IChannel *channel = it->second;
            channels_joined += channel->getName() + ",";
        }

        client->send("PART " + channels_joined);
        return channels->clear();
    }

    std::vector<std::string> channels = Lib::Smanip::split(args->at(0), ",");
    std::vector<std::string> keys = args->size() == 2 ? Lib::Smanip::split(args->at(1), ",") : std::vector<std::string>();

    for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
        std::string channel_name = *it;
        std::string key = "";

        if (keys.size() > 0) {
            key = keys.at(0);
            keys.erase(keys.begin());
        }

        IChannel *channel = server->getChannel(channel_name);

        if (channel == NULL) {
            channel = server->createChannel(channel_name);
        }

        if (channel->isBanned(client)) {
            return client->send("474 " + client->getNickname() + " " + channel->getName() + " :Cannot join channel (+b)");
        }

        if (channel->isKeyProtected() && channel->getKey() != key) {
            return client->send("475 " + client->getNickname() + " " + channel->getName() + " :Cannot join channel (+k)");
        }

        if (channel->isMember(client)) {
            return client->send("443 " + client->getNickname() + " " + channel->getName() + " :is already on channel");
        }

        if (channel->getMemberCount() == 0) {
            channel->setOperator(client, true);
        }

        channel->addMember(client);
        client->addChannel(channel);

        channel->send(":" + client->getMessageName() + " JOIN " + channel->getName());
                
        CommandArguments channelArgs;
        channelArgs.push_back(channel->getName());

        handleTOPIC(server, client, &channelArgs);
        handleNAMES(server, client, &channelArgs);
    }
}