#pragma once

#include <string>
#include <sstream>
#include <ctime>
#include <iostream>

namespace IRC {
namespace Lib {
class Logger {
 public:
    enum Level {
        DEBUG,
        INFO,
        WARN,
        ERROR,
    };

    std::stringstream _ss;
    Level _level;

 public:
    explicit Logger(Level level) {
        _level = level;
        _ss << getTime() << " | " << levelToString(level) << " | ";
    }

    template<class T>
    Logger &operator<<(const T &t) {
        _ss << t;
        return *this;
    }

    ~Logger() {
        // ToDo: Add a way to set logging level
        // if (_level <= DEBUG) {
        //     return;
        // }
        if (_level == ERROR) {
            std::cerr << _ss.str() << std::endl;
        } else {
            std::cout << _ss.str() << std::endl;
        }
    }

    static const std::string getTime() {
        struct tm newtime;
        time_t ltime;
        char buf[64];

        ltime = time(&ltime);
        localtime_r(&ltime, &newtime);

        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &newtime);
        std::string str(buf);

        return str;
    }

    #ifdef LOG_COLOR
    static const std::string levelToString(Level level) {
        switch (level) {
            case DEBUG:
                return "\033[1;32mDEBUG\033[0m";
            case INFO:
                return "\033[1;34mINFO \033[0m";
            case WARN:
                return "\033[1;33mWARN \033[0m";
            case ERROR:
                return "\033[1;31mERROR\033[0m";
        }
    }
    #endif

    #ifndef LOG_COLOR
    static const std::string levelToString(Level level) {
        switch (level) {
            case DEBUG:
                return "DEBUG";
            case INFO:
                return "INFO ";
            case WARN:
                return "WARN ";
            case ERROR:
                return "ERROR";
        }
    }
    #endif
};
}  // namespace Lib
}  // namespace IRC
