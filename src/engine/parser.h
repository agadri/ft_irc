#pragma once

#include <command/command.h>

#include <string>
#include <utility>
#include <vector>

namespace IRC {
namespace Parser {

typedef std::pair<Command::ECommand, std::vector<std::string> > CommandPacket;

std::vector<CommandPacket> parse(std::string *buf);

}
}
