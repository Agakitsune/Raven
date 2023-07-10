/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** If
*/

#pragma once

#include "Block.hpp"
#include "Expression.hpp"

namespace raven {

    class If {
        Expression _cond;
        Block _blk;
        std::variant<
            std::monostate,
            std::shared_ptr<Block>,
            std::shared_ptr<If>
        > _else;

        public:
            If() = default;
            ~If() = default;

            void unserializeIf(Input &input);

            const Expression &getCondition() const;
            const Block &getBlock() const;

            bool hasAlternate() const;
            size_t alternateIndex() const;

            template<typename T>
            requires std::is_same_v<T, Block> || std::is_same_v<T, If>
            const T &getAlternate() const {
                return *std::get<std::shared_ptr<T>>(_else);
            }

    };

}

std::ostream &operator<<(std::ostream &s, const raven::If &i);
