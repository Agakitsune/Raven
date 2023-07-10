/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** ModRM
*/

#include "asm/ModRM.hpp"

namespace raven::ASM {

    ModRM &ModRM::setMod(const Mod &mod) {
        _mod = mod;
        return *this;
    }

    ModRM &ModRM::setReg(const Register &reg) {
        _reg = reg;
        return *this;
    }

    ModRM &ModRM::setRm(const Register &rm) {
        _rm = rm;
        return *this;
    }

    uint8_t ModRM::getByte() const {
        uint8_t reg = _reg.reg;
        uint8_t rm = _rm.reg;
        uint8_t value = 0;
        value |= (static_cast<uint8_t>(_mod) & 0b11) << 6;
        value |= (reg & 0b111) << 3;
        value |= rm & 0b111;
        return value;
    }

}
