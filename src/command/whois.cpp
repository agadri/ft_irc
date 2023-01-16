/*
    Command: WHOIS
    Parameters: [ <target> ] <mask> *( "," <mask> )

    This command is used to query information about particular user.
    The server will answer this command with several numeric messages
    indicating different statuses of each user which matches the mask (if
    you are entitled to see them).  If no wildcard is present in the
    <mask>, any information about that nick which you are allowed to see
    is presented.

    If the <target> parameter is specified, it sends the query to a
    specific server.  It is useful if you want to know how long the user
    in question has been idle as only local server (i.e., the server the
    user is directly connected to) knows that information, while
    everything else is globally known.

    Wildcards are allowed in the <target> parameter.

    Numeric Replies:

            ERR_NOSUCHSERVER              ERR_NONICKNAMEGIVEN
            RPL_WHOISUSER                 RPL_WHOISCHANNELS
            RPL_WHOISCHANNELS             RPL_WHOISSERVER
            RPL_AWAY                      RPL_WHOISOPERATOR
            RPL_WHOISIDLE                 ERR_NOSUCHNICK
            RPL_ENDOFWHOIS

    Examples:

    WHOIS wiz                       ; return available user information
                                    about nick WiZ

    WHOIS eff.org trillian          ; ask server eff.org for user
                                    information  about trillian
*/

#include <command/command.h>

void IRC::Command::handleWHOIS(IServer *server, Client *client, CommandArguments *args) {
    if (args->size() < 1) {
        return client->send("431 " + client->getNickname() + " :No nickname given");
    }

    Client *target = reinterpret_cast<Client *>(server->getClient(args->at(0)));
    if (target == NULL) {
        return client->send("401 " + client->getNickname() + " " + args->at(0) + " :No such nick");
    }

    client->send("311 " + client->getNickname() + " " + target->getNickname() + " " + target->getUsername() + " " + server->getHostname() + " * :" + target->getRealname());

    client->send("312 " + client->getNickname() + " " + target->getNickname() + " " + server->getHostname() + " :" + server->getServername());

    if (target->isOperator()) {
        client->send("313 " + target->getNickname() + " :is an IRC operator");
    }

    std::map<std::string, IChannel *> *channels = target->getChannels();
    for (std::map<std::string, IChannel *>::const_iterator it = channels->begin(); it != channels->end(); ++it) {
        client->send("319 " + client->getNickname() + " " + target->getNickname() + " :" + it->second->getName());
    }

    client->send("318 " + target->getNickname() + " :End of WHOIS list");
}
