/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Array
*/

#pragma once

#include <vector>
#include <memory>

#include "Input.hpp"

namespace raven {

    class Expression;

    class Array {
        std::vector<std::shared_ptr<Expression>> _exps;

        public:
            Array() = default;
            ~Array() = default;

            void unserializeArray(Input &input);

            const std::vector<std::shared_ptr<Expression>> &getExpressions() const;

    };

}

std::ostream &operator<<(std::ostream &s, const raven::Array &array);
