/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Statement
*/

#pragma once

#include <variant>

#include "Block.hpp"
#include "Expression.hpp"
#include "Declaration.hpp"
#include "While.hpp"
#include "Return.hpp"
#include "If.hpp"

namespace raven {

    enum class StatementType {
        Block,
        Expression,
        Declaration,
        If,
        Loop,
        Return,
    };

    class Statement {
        std::variant<
            Block,
            Expression,
            Declaration,
            If,
            While,
            Return
        > _value;

        public:
            Statement() = default;
            ~Statement() = default;

            template<typename T>
            void setValue(T value) {
                _value = value;
            }

            template<typename T>
            operator T() const {
                return std::get<T>(_value);
            }

            constexpr StatementType getVariant() const {
                return static_cast<StatementType>(_value.index());
            }

    };

}

std::ostream &operator<<(std::ostream &s, const raven::Statement &statement);
