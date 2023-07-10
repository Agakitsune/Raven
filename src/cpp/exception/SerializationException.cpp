/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** SerializationException
*/

#include "exception/SerializationException.hpp"

namespace raven::exception {

    SerializationException::SerializationException(const std::string &message): message("SerializationException: " + message) {}

    const char *SerializationException::what() const noexcept {
        return message.c_str();
    }

}
