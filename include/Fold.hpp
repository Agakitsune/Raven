/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Fold
*/

#pragma once

#include <memory>

#include "Input.hpp"

namespace raven {

    class Expression;

    class Fold {
        std::shared_ptr<Expression> _exp;

        public:
            Fold() = default;
            ~Fold() = default;

            void unserializeFold(Input &input);

            const Expression &getExpression() const;
            const std::shared_ptr<Expression> &getExpressionPtr() const;

    };

}

std::ostream &operator<<(std::ostream &s, const raven::Fold &fold);
