/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Log
*/

#pragma once

#include <iostream>
#include <string>
#include <sstream>

namespace raven::log {

    class Log {
        std::string _type;
        std::string _escape;

        std::stringstream _stream;

        public:
            Log(const std::string &type, const std::string &escape = "");
            virtual ~Log() = default;

            const std::string &getType() const;
            const std::string &getEscape() const;
            std::stringstream &getStream();

    };

    static Log info("Info", "36");
    static Log warn("Warn", "33");
    static Log error("Error", "31");
    static Log debug("Debug", "35");

    Log &endl(Log &log);

}

raven::log::Log &operator<<(raven::log::Log &log, const std::string &str);
raven::log::Log &operator<<(raven::log::Log &log, const char *str);
raven::log::Log &operator<<(raven::log::Log &log, const bool &value);

template<typename T>
requires std::is_arithmetic_v<T>
raven::log::Log &operator<<(raven::log::Log &log, const T &value) {
    log.getStream() << value;
    return log;
}

raven::log::Log &operator<<(raven::log::Log &log, raven::log::Log &(*func)(raven::log::Log &));
raven::log::Log &operator<<(raven::log::Log &log, std::ios_base &(*func)(std::ios_base &) );
