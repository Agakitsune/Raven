/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Set
*/

#include <iostream>

#include "asm/Set.hpp"

namespace raven::ASM {

    Set::Set(const Register &value, const Condition &condition) : _value(value), _cond(condition) {
        update();
    }

    void Set::update() {

        uint8_t code[] = {
            0x00, // unused, just to keep the same index
            0x97,
            0x93,
            0x92,
            0x96,
            0x92,
            0x00, // unused, just to keep the same index
            0x00, // unused, just to keep the same index
            0x94,
            0x9f,
            0x9d,
            0x9c,
            0x9e,
            0x96,
            0x92,
            0x93,
            0x97,
            0x93,
            0x95,
            0x9e,
            0x9c,
            0x9d,
            0x9f,
            0x91,
            0x9b,
            0x99,
            0x95,
            0x90,
            0x9a,
            0x9a,
            0x9b,
            0x98,
            0x94
        };

        opcode2 = 0;

        if (_cond != None) {
            if (_cond == ZeroECX || _cond == ZeroRCX)
                throw exception::ASMException("Invalid Set condition, did you see some values ?");
            opcode = 0x0f;
            opcode2 = code[static_cast<int>(_cond)];
            _rm = ModRM().setMod(Mod::Mod03).setReg(RAX).setRm(_value);
            if (_value.useREX)
                _rex = REX().B(_value.extend);
            else
                _rex = std::nullopt;
        } else {
            throw exception::ASMException("Invalid Set condition, did you see some values ?");
        }

    }

    size_t Set::getOpcodeSize() const {
        return 2;
    }

    void Set::dumpOpcode(std::vector<uint8_t> &data) const {
        data.push_back(opcode);
        data.push_back(opcode2);
    }

    void Set::dumpOther(std::vector<uint8_t> &data) const {
    }

}
