/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Call
*/

#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Input.hpp"

namespace raven {

    class Expression;

    class Call {
        std::string _input;
        std::vector<std::shared_ptr<Expression>> _args;

        public:
            Call() = default;
            ~Call() = default;

            void unserializeCall(Input &input);

            const std::string &getName() const;
            const std::vector<std::shared_ptr<Expression>> &getArgs() const;

    };

}

std::ostream &operator<<(std::ostream &s, const raven::Call &call);
