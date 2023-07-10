/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** ASMException
*/

#include "exception/ASMException.hpp"

namespace raven::exception {

    ASMException::ASMException(const std::string &message): _msg("ASMException: " + message) {}

    const char *ASMException::what() const noexcept {
        return _msg.c_str();
    }

}
