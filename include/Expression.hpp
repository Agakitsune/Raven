/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Expression
*/

#pragma once

#include <variant>

#include "Literal.hpp"
#include "Identifier.hpp"
#include "Operation.hpp"
#include "Call.hpp"
#include "Fold.hpp"
#include "Index.hpp"
#include "Array.hpp"

namespace raven {

    enum class ExpressionType {
        ExpLiteral,
        ExpIdentifier,
        ExpOperation,
        ExpCall,
        ExpFold,
        ExpIndex,
        ExpArray
    };

    class Expression {
        std::variant<
            Literal,
            Identifier,
            Operation,
            Call,
            Fold,
            Index,
            Array
        > _value;

        static void generateInput(const std::shared_ptr<Expression> &expression, std::vector<std::variant<std::shared_ptr<Expression>, OperationType>> &input);

        public:
            Expression() = default;
            ~Expression() = default;

            template<typename T>
            void setValue(T value) {
                _value = value;
            }

            template<typename T>
            operator T() const {
                return std::get<T>(_value);
            }

            constexpr ExpressionType getVariant() const {
                return static_cast<ExpressionType>(_value.index());
            }

            static Expression order(const Expression &operation);

    };

}

std::ostream &operator<<(std::ostream &s, const raven::Expression &expression);
