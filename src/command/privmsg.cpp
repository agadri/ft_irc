/*
    Command: PRIVMSG
    Parameters: <msgtarget> <text to be sent>

    PRIVMSG is used to send private messages between users, as well as to
    send messages to channels.  <msgtarget> is usually the nickname of
    the recipient of the message, or a channel name.

    The <msgtarget> parameter may also be a host mask (#<mask>) or server
    mask ($<mask>).  In both cases the server will only send the PRIVMSG
    to those who have a server or host matching the mask.  The mask MUST
    have at least 1 (one) "." in it and no wildcards following the last
    ".".  This requirement exists to prevent people sending messages to
    "#*" or "$*", which would broadcast to all users.  Wildcards are the
    '*' and '?'  characters.  This extension to the PRIVMSG command is
    only available to operators.

    Numeric Replies:

            ERR_NORECIPIENT                 ERR_NOTEXTTOSEND
            ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
            ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
            ERR_NOSUCHNICK
            RPL_AWAY

    Examples:

    :Angel!wings@irc.org PRIVMSG Wiz :Are you receiving this message ?
                                    ; Message from Angel to Wiz.

    PRIVMSG Angel :yes I'm receiving it !
                                    ; Command to send a message to Angel.

    PRIVMSG jto@tolsun.oulu.fi :Hello !
                                    ; Command to send a message to a user
                                    on server tolsun.oulu.fi with
                                    username of "jto".

    PRIVMSG kalt%millennium.stealth.net@irc.stealth.net :Are you a frog?
                                    ; Message to a user on server
                                    irc.stealth.net with username of
                                    "kalt", and connected from the host
                                    millennium.stealth.net.

    PRIVMSG kalt%millennium.stealth.net :Do you like cheese?
                                    ; Message to a user on the local
                                    server with username of "kalt", and
                                    connected from the host
                                    millennium.stealth.net.

    PRIVMSG Wiz!jto@tolsun.oulu.fi :Hello !
                                    ; Message to the user with nickname
                                    Wiz who is connected from the host
                                    tolsun.oulu.fi and has the username
                                    "jto".

    PRIVMSG $*.fi :Server tolsun.oulu.fi rebooting.
                                    ; Message to everyone on a server
                                    which has a name matching *.fi.

    PRIVMSG #*.edu :NSFNet is undergoing work, expect interruptions
                                    ; Message to all users who come from
                                    a host which has a name matching
                                    *.edu.
*/

#include <command/command.h>
#include <engine/ichannel.h>

void IRC::Command::handlePRIVMSG(IServer *server, Client *client, CommandArguments *args) {
    if (args->size() == 0) {
        return client->send("411 PRIVMSG :No recipient given");
    }

    if (args->size() == 1) {
        return client->send("412 PRIVMSG :No text to send");
    }

    std::string botMsg = server->bot.eval(args->at(1), client->getNickname());
    if (botMsg.size() > 0) {
        return client->send(botMsg);
    }
    if (args->front() == "IRCBOT") {
        if (botMsg.size() == 0) {
            client->send(":IRCBOT " + client->getNickname() + " :No such command - Use !help for a list of commands");
        }
        return;
    }

    if (args->front().at(0) == '#') {
        IChannel *channel = server->getChannel(args->front());

        if (channel == NULL) {
            return client->send("403 PRIVMSG " + args->front() + " :No such channel");
        }

        if (!channel->isMember(client)) {
            return client->send("404 PRIVMSG " + args->front() + " :Cannot send to channel");
        }

        std::string message = "";
        for (unsigned int i = 1; i < args->size(); i++) {
            message += args->at(i) + " ";
        }

        if (message[0] == ':') {
            message = message.substr(1);
        }

        return channel->send(":" + client->getMessageName() + " PRIVMSG " + args->front() + " :" + message, client);
    } else {
        IClient *target = server->getClient(args->front());
        if (target == NULL) {
            return client->send("401 PRIVMSG " + args->front() + " :No such nick");
        }

        std::string dccTarget = ":ADCC";
        dccTarget[1] = 1;

        std::string message = "";
        for (unsigned int i = 1; i < args->size(); i++) {
            message += args->at(i) + " ";
        }

        if (args->at(1) == dccTarget && args->at(2) == "SEND") {
            message = dccTarget + " SEND " + args->at(3) + " " + args->at(4) + " " + args->at(5) + " " + args->at(6);
        }

        if (message[0] == ':') {
            message = message.substr(1);
        }
        return target->send(":" + client->getMessageName() + " PRIVMSG " + args->front() + " :" + message);
    }
}
