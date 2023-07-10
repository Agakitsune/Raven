/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Index
*/

#pragma once

#include <string>
#include <memory>

#include "Input.hpp"

namespace raven {

    class Expression;

    class Index {
        std::string _name;
        std::shared_ptr<Expression> _exp;

        public:
            Index() = default;
            ~Index() = default;

            void unserializeIndex(Input &input);

            const std::string &getName() const;
            const Expression &getExpression() const;
            const std::shared_ptr<Expression> &getExpressionPtr() const;

    };

}

std::ostream &operator<<(std::ostream &s, const raven::Index &index);
