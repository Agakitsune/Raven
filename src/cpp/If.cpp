/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** If
*/

#include "If.hpp"
#include "Unserializer.hpp"

namespace raven {

    void If::unserializeIf(Input &input) {
        _cond = Expression::order(unserialize<Expression>(input));
        _blk.unserializeBlk(input);
        if (unserialize<uint8_t>(input)) {
            if (unserialize<uint8_t>(input)) {
                auto ptr = std::make_shared<If>();
                input.consume(1); // There is a issue with the Serializer
                ptr->unserializeIf(input);
                _else = ptr;
            } else {
                auto ptr = std::make_shared<Block>();
                ptr->unserializeBlk(input);
                _else = ptr;
            }
        }
    }

    const Expression &If::getCondition() const {
        return _cond;
    }

    const Block &If::getBlock() const {
        return _blk;
    }

    bool If::hasAlternate() const {
        return _else.index() != 0;
    }

    size_t If::alternateIndex() const {
        return _else.index();
    }

}

std::ostream &operator<<(std::ostream &s, const raven::If &i) {
    s << "if(" << i.getCondition() << ") ";
    s << i.getBlock();
    if (i.hasAlternate()) {
        s << " else ";
        switch (i.alternateIndex()) {
            case 1:
                s << i.getAlternate<raven::Block>();
                break;
            case 2:
                s << i.getAlternate<raven::If>();
                break;
        }
    }
    return s;
}
