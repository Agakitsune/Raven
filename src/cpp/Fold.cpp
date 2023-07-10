/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Fold
*/

#include "Fold.hpp"
#include "Expression.hpp"
#include "Unserializer.hpp"

namespace raven {

    void Fold::unserializeFold(Input &input) {
        _exp = std::make_shared<Expression>(unserialize<Expression>(input));
    }

    const Expression &Fold::getExpression() const {
        return *_exp;
    }

    const std::shared_ptr<Expression> &Fold::getExpressionPtr() const {
        return _exp;
    }

}

std::ostream &operator<<(std::ostream &s, const raven::Fold &fold) {
    return s << "(" << fold.getExpression() << ")";
}
