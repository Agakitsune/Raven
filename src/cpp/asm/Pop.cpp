/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Pop
*/

#include "asm/Pop.hpp"

namespace raven::ASM {

    void Pop::update() {

        clearLegacy();

        switch (_register.size) {
            case S8:
                throw exception::ASMException("Cannot push into an 8 Bit register");
            case S32:
                throw exception::ASMException("Poping an 32 bit register is not valid in 64Bit");
            case S16:
                setLegacy(OPOverride);
            case S64:
                opcode = 0x58 + _register.reg;
                if (_register.useREX)
                    _rex = REX().B(_register.extend);
                else
                    _rex = std::nullopt;
                break;
        }

    }

    Pop::Pop(const Register &reg) {
        setRegister(reg);
    }

    size_t Pop::getOpcodeSize() const {
        return 1;
    }

    void Pop::dumpOpcode(std::vector<uint8_t> &data) const {
        if (opcode == 0)
            throw exception::ASMException("Invalid Pop opcode, did you see some values ?");
        data.push_back(opcode);
    }

    void Pop::dumpOther(std::vector<uint8_t> &data) const {
        (void)data;
    }

    Pop &Pop::setRegister(const Register &reg) {
        _register = reg;
        update();
        return *this;
    }

}
