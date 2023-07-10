/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Return
*/

#include "Return.hpp"
#include "Unserializer.hpp"

namespace raven {

    void Return::unserializeRet(Input &input) {
        if (unserialize<uint8_t>(input))
            _exp = Expression::order(unserialize<Expression>(input));
        else
            _exp = std::nullopt;
    }

    bool Return::hasExpression() const {
        return _exp.has_value();
    }

    const Expression &Return::getExpression() const {
        return _exp.value();
    }

}

std::ostream &operator<<(std::ostream &s, const raven::Return &ret) {
    s << "return ";
    if (ret.hasExpression())
        s << ret.getExpression();
    return s;
}
