/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Operation
*/

#include "Operation.hpp"
#include "Expression.hpp"
#include "Unserializer.hpp"

namespace raven {

    Operation::Operation(const OperationType &type) : _type(type) {}

    void Operation::unserializeOp(Input &input) {
        uint8_t op = unserialize<uint8_t>(input);
        _type = static_cast<OperationType>(op);
        _exp1 = std::make_shared<Expression>(unserialize<Expression>(input));
        switch(_type) {
            case OperationType::UnaryBitwiseNot:
            case OperationType::UnaryArithmeticMinus:
            case OperationType::UnaryLogicalNot:
            case OperationType::IncrementPost:
            case OperationType::DecrementPost:
            case OperationType::IncrementPre:
            case OperationType::DecrementPre:
                return;
        }
        _exp2 = std::make_shared<Expression>(unserialize<Expression>(input));
    }

    void Operation::setFirstExpression(const std::shared_ptr<Expression> &expression) {
        _exp1 = expression;
    }

    void Operation::setSecondExpression(const std::shared_ptr<Expression> &expression) {
        _exp2 = expression;
    }

    OperationType Operation::getType() const {
        return _type;
    }

    const Expression &Operation::getFirstExpression() const {
        return *_exp1;
    }

    const Expression &Operation::getSecondExpression() const {
        return *_exp2;
    }

    const std::shared_ptr<Expression> &Operation::getFirstExpressionPtr() const {
        return _exp1;
    }

    const std::shared_ptr<Expression> &Operation::getSecondExpressionPtr() const {
        return _exp2;
    }

}

std::ostream &operator<<(std::ostream &s, const raven::OperationType &type) {
    switch (type) {
        case raven::OperationType::UnaryBitwiseNot:
            return s << "~";
        case raven::OperationType::BinaryBitwiseAnd:
            return s << "&";
        case raven::OperationType::BinaryBitwiseOr:
            return s << "|";
        case raven::OperationType::BinaryBitwiseXor:
            return s << "^";
        case raven::OperationType::BinaryBitwiseLShift:
            return s << "<<";
        case raven::OperationType::BinaryBitwiseRShift:
            return s << ">>";
        case raven::OperationType::BinaryBitwiseLRotShift:
            return s << "<<<";
        case raven::OperationType::BinaryBitwiseRRotShift:
            return s << ">>>";
        case raven::OperationType::UnaryArithmeticMinus:
            return s << "-";
        case raven::OperationType::BinaryArithmeticAdd:
            return s << "+";
        case raven::OperationType::BinaryArithmeticMinus:
            return s << "-";
        case raven::OperationType::BinaryArithmeticMultiply:
            return s << "*";
        case raven::OperationType::BinaryArithmeticDivide:
            return s << "/";
        case raven::OperationType::BinaryArithmeticModulo:
            return s << "%";
        case raven::OperationType::BinaryArithmeticPower:
            return s << "**";
        case raven::OperationType::UnaryLogicalNot:
            return s << "!";
        case raven::OperationType::BinaryLogicalEqual:
            return s << "==";
        case raven::OperationType::BinaryLogicalNotEqual:
            return s << "!=";
        case raven::OperationType::BinaryLogicalLower:
            return s << "<";
        case raven::OperationType::BinaryLogicalGreater:
            return s << ">";
        case raven::OperationType::BinaryLogicalLowerEq:
            return s << "<=";
        case raven::OperationType::BinaryLogicalGreaterEq:
            return s << ">=";
        case raven::OperationType::BinaryLogicalAnd:
            return s << "&&";
        case raven::OperationType::BinaryLogicalOr:
            return s << "||";
        case raven::OperationType::BinaryLogicalXor:
            return s << "^|";
        case raven::OperationType::IncrementPost:
            return s << "++";
        case raven::OperationType::DecrementPost:
            return s << "--";
        case raven::OperationType::IncrementPre:
            return s << "++";
        case raven::OperationType::DecrementPre:
            return s << "--";
        case raven::OperationType::Assignment:
            return s << "=";
        case raven::OperationType::AddAssignment:
            return s << "+=";
        case raven::OperationType::MinusAssignment:
            return s << "-=";
        case raven::OperationType::MultiplyAssignment:
            return s << "*=";
        case raven::OperationType::DivideAssignment:
            return s << "/=";
        case raven::OperationType::ModuloAssignment:
            return s << "%=";
        case raven::OperationType::PowerAssignment:
            return s << "**=";
        case raven::OperationType::BitwiseAndAssignment:
            return s << "&=";
        case raven::OperationType::BitwiseOrAssignment:
            return s << "|=";
        case raven::OperationType::BitwiseXorAssignment:
            return s << "^=";
        case raven::OperationType::BitwiseRShiftAssignment:
            return s << "<<=";
        case raven::OperationType::BitwiseLShiftAssignment:
            return s << ">>=";
        case raven::OperationType::BitwiseRRotShiftAssignment:
            return s << "<<<=";
        case raven::OperationType::BitwiseLRotShiftAssignment:
            return s << ">>>=";

        case raven::OperationType::__LeftParenthesis:
            return s << "(";
        case raven::OperationType::__RightParenthesis:
            return s << ")";
    }
    return s;
}

std::ostream &operator<<(std::ostream &s, const raven::Operation &op) {
    switch (op.getType()) {
        case raven::OperationType::UnaryBitwiseNot:
        case raven::OperationType::UnaryArithmeticMinus:
        case raven::OperationType::UnaryLogicalNot:
        case raven::OperationType::IncrementPre:
        case raven::OperationType::DecrementPre:
            return s << op.getType() << op.getFirstExpression();
        case raven::OperationType::IncrementPost:
        case raven::OperationType::DecrementPost:
            return s << op.getFirstExpression() << op.getType();
        default:
            return s << op.getFirstExpression() << " " << op.getType() << " " << op.getSecondExpression();
    }
}
