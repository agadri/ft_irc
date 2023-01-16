#include <pkg/logger.h>
#include <engine/parser.h>
#include <command/command.h>

#include <string>
#include <vector>
#include <utility>
#include <iostream>

using IRC::Lib::Logger;

std::vector<IRC::Parser::CommandPacket> IRC::Parser::parse(std::string *buf) {
    std::vector<IRC::Parser::CommandPacket> commands;

    /* If the buffer is empty, not complete or illegal, we wait for a next input */
    if (buf == NULL || buf->size() == 0 || buf->find("\r\n") == std::string::npos) {
        std::cout << "Buffer is incomplete (" << buf->size() << " pending bytes)" << std::endl;
        return commands;
    }

    while (buf->find("\r\n") != std::string::npos) {
        std::string packet = buf->substr(0, buf->find("\r\n"));
        
        *buf = buf->substr(buf->find("\r\n") + 2, buf->size());
        if (packet == "" || packet == "\r\n") {
            continue;
        }

        /* Extract the command from the packet */
        std::string command = packet.substr(0, packet.find(' '));
        Command::ECommand cmd = Command::getCommand(command);

        /* If the command is unknown, we ignore it but dump the data */
        if (cmd == Command::UNDEFINED) {
            /* Create a tcpdump-like output */
            std::stringstream tcpdump;
            for (std::size_t i = 0; i < command.size(); i++) {
                tcpdump << std::hex << static_cast<int>(command[i]) << " ";
            }

            Logger(Logger::ERROR) << "Unknown command: |" << command << "| " << tcpdump.str();
            continue;
        }

        /* Extract the arguments from the packet */
        Command::CommandArguments args;
        if (packet.size() > command.size() + 1) {
            while (packet.find(' ') != std::string::npos) {
                packet.erase(0, packet.find(' ') + 1);
                args.push_back(packet.substr(0, packet.find(' ')));
            }
        }

        /* Create the command packet */
        CommandPacket pair = std::make_pair(Command::getCommand(command), args);

        std::stringstream arguments;
        for (std::size_t i = 0; i < args.size(); i++) {
            arguments << args[i] << " ";
        }

        Logger(Logger::DEBUG) << "Parsed command : [" << command << "] " << arguments.str();

        /* Add the command packet to the list */
        commands.push_back(pair);
    }

    return commands;
}
