#pragma once

#include <vector>
#include <string>
#include <sstream>

namespace IRC {
namespace Lib {
namespace Smanip {

static inline std::string toString(const int &i) {
    std::stringstream ss;

    ss << i;
    return ss.str();
}

static inline std::vector<std::string> split(const std::string &s, const std::string &sep) {
    std::vector<std::string> ret;

    if (s.empty()) {
        return ret;
    }

    std::string::size_type prev_pos = 0, pos = 0;

    while ((pos = s.find(sep, pos)) != std::string::npos) {
        std::string substring(s.substr(prev_pos, pos - prev_pos));
        ret.push_back(substring);
        prev_pos = ++pos;
    }
    ret.push_back(s.substr(prev_pos, pos - prev_pos));
    return ret;
}

static inline std::string toUpper(const std::string &s) {
    std::string ret = s;

    for (std::string::iterator it = ret.begin(); it != ret.end(); ++it) {
        if (*it >= 'a' && *it <= 'z') {
            *it -= 32;
        }
    }
    return ret;
}

}  // namespace Smanip
}  // namespace Lib
}  // namespace IRC
