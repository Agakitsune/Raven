/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Operation
*/

#pragma once

#include <memory>
#include <unordered_map>
#include <queue>
#include <stack>
#include <variant>
#include <unordered_set>

#include "Input.hpp"

namespace raven {

    class Expression;

    enum class OperationType {
        UnaryBitwiseNot,
        BinaryBitwiseAnd,
        BinaryBitwiseOr,
        BinaryBitwiseXor,
        BinaryBitwiseLShift,
        BinaryBitwiseRShift,
        BinaryBitwiseLRotShift,
        BinaryBitwiseRRotShift,
        UnaryArithmeticMinus,
        BinaryArithmeticAdd,
        BinaryArithmeticMinus,
        BinaryArithmeticMultiply,
        BinaryArithmeticDivide,
        BinaryArithmeticModulo,
        BinaryArithmeticPower,
        UnaryLogicalNot,
        BinaryLogicalEqual,
        BinaryLogicalNotEqual,
        BinaryLogicalLower,
        BinaryLogicalGreater,
        BinaryLogicalLowerEq,
        BinaryLogicalGreaterEq,
        BinaryLogicalAnd,
        BinaryLogicalOr,
        BinaryLogicalXor,
        IncrementPost,
        DecrementPost,
        IncrementPre,
        DecrementPre,
        Assignment,
        AddAssignment,
        MinusAssignment,
        MultiplyAssignment,
        DivideAssignment,
        ModuloAssignment,
        PowerAssignment,
        BitwiseAndAssignment,
        BitwiseOrAssignment,
        BitwiseXorAssignment,
        BitwiseRShiftAssignment,
        BitwiseLShiftAssignment,
        BitwiseRRotShiftAssignment,
        BitwiseLRotShiftAssignment,

        __LeftParenthesis, // to handle Fold Expression with Shunting Yard
        __RightParenthesis  // to handle Fold Expression with Shunting Yard
    };

    class Operation {
        OperationType _type;
        std::shared_ptr<Expression> _exp1;
        std::shared_ptr<Expression> _exp2;

        public:
            Operation() = default;
            Operation(const OperationType &type);
            ~Operation() = default;

            void unserializeOp(Input &input);

            void setFirstExpression(const std::shared_ptr<Expression> &exp);
            void setSecondExpression(const std::shared_ptr<Expression> &exp);

            OperationType getType() const;
            const Expression &getFirstExpression() const;
            const Expression &getSecondExpression() const;

            const std::shared_ptr<Expression> &getFirstExpressionPtr() const;
            const std::shared_ptr<Expression> &getSecondExpressionPtr() const;

    };

    struct ShuntingOperator {
        OperationType type;
        size_t precedence;
        bool rightToLeft;
    };

}

std::ostream &operator<<(std::ostream &s, const raven::OperationType &type);
std::ostream &operator<<(std::ostream &s, const raven::Operation &op);
