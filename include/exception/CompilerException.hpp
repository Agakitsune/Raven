/*
** EPITECH PROJECT, 2023
** Untitled (Workspace)
** File description:
** CompilerException
*/

#pragma once

#include <string>
#include <exception>

namespace raven::exception {

    class CompilerException : public std::exception {
        std::string _message;

        public:
            CompilerException(const std::string &message);
            ~CompilerException() = default;

            const char *what() const noexcept override;

    };

}