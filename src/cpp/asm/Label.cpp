/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Label
*/

#include "asm/Label.hpp"

namespace raven::ASM {

    Label &Label::setOffset(const int64_t &offset) {
        _offset = offset;
        return *this;
    }

    int64_t Label::getOffset() const {
        return _offset;
    }

    Immediate Label::relative(const int64_t pos, const ImmediateSize &size) {
        return Immediate( _offset - pos).setSize(size);
    }

}
