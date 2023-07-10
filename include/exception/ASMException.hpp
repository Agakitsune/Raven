/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** ASMException
*/

#pragma once

#include <exception>
#include <string>

namespace raven::exception {

    class ASMException : public std::exception {
        std::string _msg;

        public:
            ASMException(const std::string &message);
            ~ASMException() = default;

            virtual const char* what() const noexcept override;

    };

}
