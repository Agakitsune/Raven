/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Push
*/

#include "asm/Push.hpp"

namespace raven::ASM {

    void Push::update() {

        clearLegacy();

        if (_value.index() == 0) {
            Register &r = std::get<Register>(_value);
            switch (r.size) {
                case S8:
                    throw exception::ASMException("Cannot push an 8 Bit register");
                case S32:
                    throw exception::ASMException("Pushing an 32 bit register is not valid in 64Bit");
                case S16:
                    setLegacy(OPOverride);
                case S64:
                    opcode = 0x50 + r.reg;
                    if (r.useREX)
                        _rex = REX().B(r.extend);
                    else
                        _rex = std::nullopt;
                    break;
            }
        } else {
            Immediate &i = std::get<Immediate>(_value);
            switch (i.getImmediateSize()) {
                case IMM8:
                    opcode = 0x6a;
                    break;
                case IMM16:
                case IMM32:
                    opcode = 0x68;
                    break;
                case IMM64:
                    throw exception::ASMException("Cannot push a 64 bit immediate value");
            }
        }

    }

    Push::Push(const Register &reg) {
        setValue(reg);
    }

    size_t Push::getOpcodeSize() const {
        return 1;
    }

    void Push::dumpOpcode(std::vector<uint8_t> &data) const {
        if (opcode == 0)
            throw exception::ASMException("Invalid Sub opcode, did you see some values ?");
        data.push_back(opcode);
    }

    void Push::dumpOther(std::vector<uint8_t> &data) const {
        (void)data;
    }

}
