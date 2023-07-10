/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Log
*/

#include "Log.hpp"

namespace raven::log {

    Log::Log(const std::string &type, const std::string &escape) : _type(type), _escape(escape) {}

    const std::string &Log::getType() const {
        return _type;
    }

    const std::string &Log::getEscape() const {
        return _escape;
    }

    std::stringstream &Log::getStream() {
        return _stream;
    }

    Log &endl(Log &log) {
        const std::string &escape = log.getEscape();
        std::string str = std::move(log.getStream().str());
        if (escape.empty())
            std::clog << "[" << log.getType() << "] " << str << std::endl;
        else
            std::clog << "[\033[" << escape << "m" << log.getType() << "\033[0m] " << str << std::endl;
        log.getStream().str("");
        return log;
    }

}

raven::log::Log &operator<<(raven::log::Log &log, const std::string &str) {
    log.getStream() << str;
    return log;
}

raven::log::Log &operator<<(raven::log::Log &log, const char *str) {
    return log << std::string(str);
}

raven::log::Log &operator<<(raven::log::Log &log, const bool &value) {
    log.getStream() << std::boolalpha << value;
    return log;
}

raven::log::Log &operator<<(raven::log::Log &log, raven::log::Log &(*func)(raven::log::Log &)) {
    return func(log);
}

raven::log::Log &operator<<(raven::log::Log &log, std::ios_base &(*func)(std::ios_base &) ) {
    log.getStream() << func;
    return log;
}
