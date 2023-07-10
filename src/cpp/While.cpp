/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** While
*/

#include "While.hpp"
#include "Unserializer.hpp"

namespace raven {

    void While::unserializeWhile(Input &input) {
        _cond = Expression::order(unserialize<Expression>(input));
        _blk.unserializeBlk(input);
    }

    const Expression &While::getCondition() const {
        return _cond;
    }

    const Block &While::getBlock() const {
        return _blk;
    }

}

std::ostream &operator<<(std::ostream &s, const raven::While &w) {
    return s << "while(" << w.getCondition() << ") " << w.getBlock();
}
