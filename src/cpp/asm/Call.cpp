/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Call
*/

#include "asm/Call.hpp"

namespace raven::ASM {

    void Call::update() {

        if (_value.index() == 1) {
            Immediate &i = std::get<Immediate>(_value);
            switch (i.getImmediateSize()) {
                case IMM8:
                case IMM16:
                case IMM64:
                    throw exception::ASMException("Invalid operand size");
                case IMM32:
                    opcode = 0xe8;
                    break;
            }
        } else {
            Register &r = std::get<Register>(_value);
            switch (r.size) {
                case S8:
                case S16:
                case S32:
                    throw exception::ASMException("Invalid operand size");
                case S64:
                    opcode = 0xff;
                    _rm = ModRM().setMod(Mod::Mod03).setReg(EDX).setRm(r);
                    if (r.useREX)
                        _rex = REX().B(r.extend);
                    else
                        _rex = std::nullopt;
            }
        }

    }

    size_t Call::getOpcodeSize() const {
        if (_value.index() == 1)
            return 5;
        return 1;
    }

    void Call::dumpOpcode(std::vector<uint8_t> &data) const {
        data.push_back(opcode);
    }

    void Call::dumpOther(std::vector<uint8_t> &data) const {
        if (_value.index() == 1)
            std::get<Immediate>(_value).dump(data);
    }

}
