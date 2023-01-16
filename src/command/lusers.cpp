/*
Command: LUSERS
   Parameters: [ <mask> [ <target> ] ]

   The LUSERS command is used to get statistics about the size of the
   IRC network.  If no parameter is given, the reply will be about the
   whole net.  If a <mask> is specified, then the reply will only
   concern the part of the network formed by the servers matching the
   mask.  Finally, if the <target> parameter is specified, the request
   is forwarded to that server which will generate the reply.

   Wildcards are allowed in the <target> parameter.

   Numeric Replies:

           RPL_LUSERCLIENT                 RPL_LUSEROP
           RPL_LUSERUNKOWN                 RPL_LUSERCHANNELS
           RPL_LUSERME                     ERR_NOSUCHSERVER
*/

#include <command/command.h>
#include <pkg/smanip.h>

void IRC::Command::handleLUSERS(IServer *server, Client *client, CommandArguments *args) {
    if (args->size() > 2) {
        return client->send("461 LUSERS :Too many parameters");
    }

    size_t cc_operators = 0;
    size_t cc_channels = 0;
    size_t cc_clients = 0;

    std::map<int, IClient *> *clients = server->getClients();
    std::map<int, IClient *>::iterator it = clients->begin();
    for (; it != clients->end(); ++it) {
        IClient *client = it->second;
        if (client->isOperator()) {
            ++cc_operators;
        }
        ++cc_clients;
    }

    std::map<std::string, IChannel *> *channels = server->getChannels();
    cc_channels = channels->size();

    client->send("251 " + client->getNickname() + " :There are " + Lib::Smanip::toString(cc_clients) + " users and 0 services on 1 servers");
    client->send("252 " + client->getNickname() + " " + Lib::Smanip::toString(cc_operators) + " :operator(s) online");
    client->send("253 " + client->getNickname() + " 0 :unknown connection(s)");
    client->send("254 " + client->getNickname() + " " + Lib::Smanip::toString(cc_channels) + " :channels formed");
    client->send("255 " + client->getNickname() + " :I have " + Lib::Smanip::toString(cc_clients) + " clients and 1 servers");
}