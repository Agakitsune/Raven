/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Expression
*/

#include <iostream>

#include "Expression.hpp"

namespace raven {

    void Expression::generateInput(const std::shared_ptr<Expression> &expression, std::vector<std::variant<std::shared_ptr<Expression>, OperationType>> &input) {
        static std::unordered_set<OperationType> unary = {
            OperationType::UnaryBitwiseNot,
            OperationType::UnaryArithmeticMinus,
            OperationType::UnaryLogicalNot,
            OperationType::IncrementPost,
            OperationType::DecrementPost,
            OperationType::IncrementPre,
            OperationType::DecrementPre,
        };
        static std::unordered_set<ExpressionType> leaf = {
            ExpressionType::ExpLiteral,
            ExpressionType::ExpIdentifier,
            ExpressionType::ExpCall,
        };

        if (leaf.contains(expression->getVariant())) {
            input.push_back(expression);
        } else if (expression->getVariant() == ExpressionType::ExpFold) {
            input.push_back(OperationType::__LeftParenthesis);
            Fold fold = static_cast<Fold>(*expression);
            generateInput(fold.getExpressionPtr(), input);
            input.push_back(OperationType::__RightParenthesis);
        } else {
            Operation op = static_cast<Operation>(*expression);
            if (unary.contains(op.getType())) {
                input.push_back(op.getType());
                generateInput(op.getFirstExpressionPtr(), input);
            } else {
                generateInput(op.getFirstExpressionPtr(), input);
                input.push_back(op.getType());
                generateInput(op.getSecondExpressionPtr(), input);
            }
        }

    }

    std::unordered_map<OperationType, ShuntingOperator> OPERATORS = {
        {OperationType::__LeftParenthesis, {OperationType::__LeftParenthesis, 255, false}},
        {OperationType::__RightParenthesis, {OperationType::__RightParenthesis, 255, false}},

        {OperationType::IncrementPost, {OperationType::IncrementPost, 1, false}},
        {OperationType::DecrementPost, {OperationType::DecrementPost, 1, false}},

        {OperationType::IncrementPre, {OperationType::IncrementPre, 2, true}},
        {OperationType::DecrementPre, {OperationType::DecrementPre, 2, true}},
        {OperationType::UnaryArithmeticMinus, {OperationType::UnaryArithmeticMinus, 2, true}},
        {OperationType::UnaryBitwiseNot, {OperationType::UnaryBitwiseNot, 2, true}},
        {OperationType::UnaryLogicalNot, {OperationType::UnaryLogicalNot, 2, true}},

        {OperationType::BinaryArithmeticPower, {OperationType::BinaryArithmeticPower, 3, true}},

        {OperationType::BinaryArithmeticMultiply, {OperationType::BinaryArithmeticMultiply, 4, false}},
        {OperationType::BinaryArithmeticDivide, {OperationType::BinaryArithmeticDivide, 4, false}},
        {OperationType::BinaryArithmeticModulo, {OperationType::BinaryArithmeticModulo, 4, false}},

        {OperationType::BinaryArithmeticAdd, {OperationType::BinaryArithmeticAdd, 5, false}},
        {OperationType::BinaryArithmeticMinus, {OperationType::BinaryArithmeticMinus, 5, false}},

        {OperationType::BinaryBitwiseLShift, {OperationType::BinaryBitwiseLShift, 5, false}},
        {OperationType::BinaryBitwiseRShift, {OperationType::BinaryBitwiseRShift, 5, false}},
        {OperationType::BinaryBitwiseLRotShift, {OperationType::BinaryBitwiseLRotShift, 5, false}},
        {OperationType::BinaryBitwiseRRotShift, {OperationType::BinaryBitwiseRRotShift, 5, false}},

        {OperationType::BinaryLogicalLower, {OperationType::BinaryLogicalLower, 6, false}},
        {OperationType::BinaryLogicalGreater, {OperationType::BinaryLogicalGreater, 6, false}},
        {OperationType::BinaryLogicalLowerEq, {OperationType::BinaryLogicalLowerEq, 6, false}},
        {OperationType::BinaryLogicalGreaterEq, {OperationType::BinaryLogicalGreaterEq, 6, false}},

        {OperationType::BinaryLogicalEqual, {OperationType::BinaryLogicalEqual, 7, false}},
        {OperationType::BinaryLogicalNotEqual, {OperationType::BinaryLogicalNotEqual, 7, false}},

        {OperationType::BinaryBitwiseAnd, {OperationType::BinaryBitwiseAnd, 8, false}},

        {OperationType::BinaryBitwiseXor, {OperationType::BinaryBitwiseXor, 9, false}},

        {OperationType::BinaryBitwiseOr, {OperationType::BinaryBitwiseOr, 10, false}},

        {OperationType::BinaryLogicalAnd, {OperationType::BinaryLogicalAnd, 11, false}},

        {OperationType::BinaryLogicalXor, {OperationType::BinaryLogicalXor, 12, false}},

        {OperationType::BinaryLogicalOr, {OperationType::BinaryLogicalOr, 13, false}},

        {OperationType::Assignment, {OperationType::Assignment, 14, true}},
        {OperationType::AddAssignment, {OperationType::AddAssignment, 14, true}},
        {OperationType::MinusAssignment, {OperationType::MinusAssignment, 14, true}},
        {OperationType::MultiplyAssignment, {OperationType::MultiplyAssignment, 14, true}},
        {OperationType::DivideAssignment, {OperationType::DivideAssignment, 14, true}},
        {OperationType::ModuloAssignment, {OperationType::ModuloAssignment, 14, true}},
        {OperationType::PowerAssignment, {OperationType::PowerAssignment, 14, true}},
        {OperationType::BitwiseAndAssignment, {OperationType::BitwiseAndAssignment, 14, true}},
        {OperationType::BitwiseOrAssignment, {OperationType::BitwiseOrAssignment, 14, true}},
        {OperationType::BitwiseXorAssignment, {OperationType::BitwiseXorAssignment, 14, true}},
        {OperationType::BitwiseRShiftAssignment, {OperationType::BitwiseRShiftAssignment, 14, true}},
        {OperationType::BitwiseLShiftAssignment, {OperationType::BitwiseLShiftAssignment, 14, true}},
        {OperationType::BitwiseRRotShiftAssignment, {OperationType::BitwiseRRotShiftAssignment, 14, true}},
        {OperationType::BitwiseLRotShiftAssignment, {OperationType::BitwiseLRotShiftAssignment, 14, true}},
    };

    Expression Expression::order(const Expression &expression) {
        std::queue<std::variant<std::shared_ptr<Expression>, ShuntingOperator>> output;
        std::stack<ShuntingOperator> operators;

        std::unordered_set<OperationType> unary;
        std::unordered_set<ExpressionType> leaf;

        unary.insert(OperationType::UnaryBitwiseNot);
        unary.insert(OperationType::UnaryArithmeticMinus);
        unary.insert(OperationType::UnaryLogicalNot);
        unary.insert(OperationType::IncrementPost);
        unary.insert(OperationType::DecrementPost);
        unary.insert(OperationType::IncrementPre);
        unary.insert(OperationType::DecrementPre);

        leaf.insert(ExpressionType::ExpLiteral);
        leaf.insert(ExpressionType::ExpIdentifier);
        leaf.insert(ExpressionType::ExpCall);

        if (leaf.contains(expression.getVariant())) {
            return expression;
        } else if (expression.getVariant() == ExpressionType::ExpFold) {
            Fold fold = static_cast<Fold>(expression);
            return order(fold.getExpression());
        }

        std::vector<std::variant<std::shared_ptr<Expression>, OperationType>> input;

        std::shared_ptr<Expression> ptr = std::make_shared<Expression>(expression);
        generateInput(ptr, input);

        for (auto &elem : input) {
            if (elem.index() == 0) {
                output.push(std::get<std::shared_ptr<Expression>>(elem));
            } else {
                ShuntingOperator in = OPERATORS[std::get<OperationType>(elem)];
                if (in.type == OperationType::__LeftParenthesis) {
                    operators.push(in);
                    continue;
                } else if (in.type == OperationType::__RightParenthesis) {
                    while (operators.top().type != OperationType::__LeftParenthesis) {
                        output.push(operators.top());
                        operators.pop();
                    }
                    operators.pop();
                    continue;
                }
                while (
                    !operators.empty() &&
                    ((operators.top().precedence < in.precedence) ||
                    ((operators.top().precedence == in.precedence) && !in.rightToLeft))
                ) {
                    output.push(operators.top());
                    operators.pop();
                }
                operators.push(in);
            }
        }
        while (!operators.empty()) {
            output.push(operators.top());
                        operators.pop();
        }

        // Shunting Yard is done, now we can build the tree

        std::stack<std::shared_ptr<Expression>> tree;

        while (!output.empty()) {
            if (output.front().index() == 0) {
                                tree.push(std::get<std::shared_ptr<Expression>>(output.front()));
            } else {
                ShuntingOperator op = std::get<ShuntingOperator>(output.front());
                                if (unary.contains(op.type)) {
                    std::shared_ptr<Expression> first = tree.top();
                    tree.pop();
                    std::shared_ptr<Expression> exp = std::make_shared<Expression>();
                    Operation out(op.type);
                    out.setFirstExpression(first);
                                        exp->setValue(out);
                    tree.push(exp);
                } else {
                    std::shared_ptr<Expression> second = tree.top();
                    tree.pop();
                    std::shared_ptr<Expression> first = tree.top();
                    tree.pop();
                    std::shared_ptr<Expression> exp = std::make_shared<Expression>();
                    Operation out(op.type);
                    out.setFirstExpression(first);
                    out.setSecondExpression(second);
                                        exp->setValue(out);
                    tree.push(exp);
                }
            }
            output.pop();
        }

        return *tree.top();
    }

}

std::ostream &operator<<(std::ostream &s, const raven::Expression &exp) {
    switch (exp.getVariant()) {
        case raven::ExpressionType::ExpLiteral:
            return s << static_cast<raven::Literal>(exp);
        case raven::ExpressionType::ExpIdentifier:
            return s << static_cast<raven::Identifier>(exp);
        case raven::ExpressionType::ExpOperation:
            return s << static_cast<raven::Operation>(exp);
        case raven::ExpressionType::ExpCall:
            return s << static_cast<raven::Call>(exp);
        case raven::ExpressionType::ExpFold:
            return s << static_cast<raven::Fold>(exp);
    }
    return s;
}
