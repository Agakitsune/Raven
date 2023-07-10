/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** While
*/

#pragma once

#include "Expression.hpp"
#include "Block.hpp"

namespace raven {

    class While {
        Expression _cond;
        Block _blk;

        public:
            While() = default;
            ~While() = default;

            void unserializeWhile(Input &input);

            const Expression &getCondition() const;
            const Block &getBlock() const;

    };

}

std::ostream &operator<<(std::ostream &s, const raven::While &w);
