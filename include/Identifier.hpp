/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Identifier
*/

#pragma once

#include <string>

#include "Input.hpp"

namespace raven {

    class Identifier {
        std::string _id;

        public:
            Identifier() = default;
            ~Identifier() = default;

            void unserializeId(Input &input);

            const std::string &getId() const;

    };

}

std::ostream &operator<<(std::ostream &s, const raven::Identifier &id);
