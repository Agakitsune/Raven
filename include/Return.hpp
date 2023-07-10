/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Return
*/

#pragma once

#include <optional>

#include "Expression.hpp"

namespace raven {

    class Return {
        std::optional<Expression> _exp;

        public:
            Return() = default;
            ~Return() = default;

            void unserializeRet(Input &input);

            bool hasExpression() const;
            const Expression &getExpression() const;

    };

}

std::ostream &operator<<(std::ostream &s, const raven::Return &ret);
