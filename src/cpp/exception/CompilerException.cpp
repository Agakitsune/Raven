/*
** EPITECH PROJECT, 2023
** Untitled (Workspace)
** File description:
** CompilerException
*/

#include "exception/CompilerException.hpp"

namespace raven::exception {

    CompilerException::CompilerException(const std::string &message) : _message(message)  {}

    const char *CompilerException::what() const noexcept {
        return _message.c_str();
    }

}
