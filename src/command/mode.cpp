/*
   Command: MODE
   Parameters: <nickname>
               *( ( "+" / "-" ) *( "i" / "w" / "o" / "O" / "r" ) )

   The user MODE's are typically changes which affect either how the
   client is seen by others or what 'extra' messages the client is sent.

   A user MODE command MUST only be accepted if both the sender of the
   message and the nickname given as a parameter are both the same.  If
   no other parameter is given, then the server will return the current
   settings for the nick.

      The available modes are as follows:

           a - user is flagged as away;
           i - marks a users as invisible;
           w - user receives wallops;
           r - restricted user connection;
           o - operator flag;
           O - local operator flag;
           s - marks a user for receipt of server notices.

   Additional modes may be available later on.

   The flag 'a' SHALL NOT be toggled by the user using the MODE command,
   instead use of the AWAY command is REQUIRED.

   If a user attempts to make themselves an operator using the "+o" or
   "+O" flag, the attempt SHOULD be ignored as users could bypass the
   authentication mechanisms of the OPER command.  There is no
   restriction, however, on anyone `deopping' themselves (using "-o" or
   "-O").

   On the other hand, if a user attempts to make themselves unrestricted
   using the "-r" flag, the attempt SHOULD be ignored.  There is no
   restriction, however, on anyone `deopping' themselves (using "+r").
   This flag is typically set by the server upon connection for
   administrative reasons.  While the restrictions imposed are left up
   to the implementation, it is typical that a restricted user not be
   allowed to change nicknames, nor make use of the channel operator
   status on channels.

   The flag 's' is obsolete but MAY still be used.

   Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_USERSDONTMATCH
           ERR_UMODEUNKNOWNFLAG            RPL_UMODEIS

   Examples:

   MODE WiZ -w                     ; Command by WiZ to turn off
                                   reception of WALLOPS messages.

   MODE Angel +i                   ; Command from Angel to make herself
                                   invisible.

   MODE WiZ -o                     ; WiZ 'deopping' (removing operator
                                   status).
*/

#include <command/command.h>

void IRC::Command::handleMODE(IServer *server, Client *client, CommandArguments *args) {
    (void)server;

    if (args->size() < 1) {
        client->send("461 " + client->getNickname() + " MODE :Not enough parameters");
        return;
    }

    if (args->size() == 1) {
        client->send("324 " + client->getNickname() + " " + args->at(0) + " " + client->getModes());
        return;
    }

    if (args->at(1)[0] == '+') {
        client->addMode(args->at(1).erase(0, 1));
    } else if (args->at(1)[0] == '-') {
        client->removeMode(args->at(1).erase(0, 1));
    } else {
        client->send("472 " + client->getNickname() + " " + args->at(2) + " :is unknown mode char to me");
    }

    if (client->getNickname() == args->at(0)) {
        client->send("324 " + client->getNickname() + " " + args->at(1) + " " + client->getModes());
    } else {
        client->send("502 " + client->getNickname() + " :Can't change mode for other users");
    }
}