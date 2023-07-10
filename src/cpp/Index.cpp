/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Index
*/

#include "Index.hpp"
#include "Expression.hpp"
#include "Unserializer.hpp"

namespace raven {

    void Index::unserializeIndex(Input &input) {
        _name = unserialize<std::string>(input);
        _exp = std::make_shared<Expression>(unserialize<Expression>(input));
    }

    const std::string &Index::getName() const {
        return _name;
    }

    const Expression &Index::getExpression() const {
        return *_exp;
    }

    const std::shared_ptr<Expression> &Index::getExpressionPtr() const {
        return _exp;
    }

}

std::ostream &operator<<(std::ostream &s, const raven::Index &index) {
    return s << index.getName() << "[" << index.getExpression() << "]";
}
