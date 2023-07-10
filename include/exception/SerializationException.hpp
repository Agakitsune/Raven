/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** SerializationException
*/

#pragma once

#include <exception>
#include <string>

namespace raven::exception {

    class SerializationException : public std::exception {
        std::string message;

        public:
            SerializationException(const std::string &message);
            virtual ~SerializationException() override = default;

            const char *what() const noexcept override;
    };

}
