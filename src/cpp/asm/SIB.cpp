/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** SIB
*/

#include "asm/SIB.hpp"

namespace raven::ASM {

    SIB &SIB::scale(uint8_t value) {
        _scale = value;
        return *this;
    }

    SIB &SIB::index(uint8_t value) {
        _index = value;
        return *this;
    }

    SIB &SIB::base(uint8_t value) {
        _base = value;
        return *this;
    }

    uint8_t SIB::getByte() const {
        return (_scale << 6) | (_index << 3) | _base;
    }

}