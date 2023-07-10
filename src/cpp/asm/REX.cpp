/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** REX
*/

#include "asm/REX.hpp"

namespace raven::ASM {

    REX &REX::W(bool value) {
        _w = value;
        return *this;
    }

    REX &REX::R(bool value) {
        _r = value;
        return *this;
    }

    REX &REX::X(bool value) {
        _x = value;
        return *this;
    }

    REX &REX::B(bool value) {
        _b = value;
        return *this;
    }

    uint8_t REX::getByte() const {
        uint8_t result = 0x40;
        result |= (_w & 0b1) << 3;
        result |= (_r & 0b1) << 2;
        result |= (_x & 0b1) << 1;
        result |= (_b & 0b1);
        return result;
    }

}
