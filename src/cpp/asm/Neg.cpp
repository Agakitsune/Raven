/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Neg
*/

#include "asm/Neg.hpp"

namespace raven::ASM {

    void Neg::update() {

        clearLegacy();

        switch (_register.size) {
            case S8:
                opcode = 0xf6;
                _rm = ModRM().setMod(Mod::Mod03).setRm(_register).setReg(EBX);
                if (_register.useREX)
                    _rex = REX().B(_register.extend);
                else
                    _rex = std::nullopt;
                break;
            case S16:
                setLegacy(OPOverride);
            case S32:
                opcode = 0xf7;
                _rm = ModRM().setMod(Mod::Mod03).setRm(_register).setReg(EBX);
                if (_register.useREX)
                    _rex = REX().B(_register.extend);
                else
                    _rex = std::nullopt;
                break;
            case S64:
                opcode = 0xf7;
                _rm = ModRM().setMod(Mod::Mod03).setRm(_register).setReg(EBX);
                _rex = REX().W(true).B(_register.extend);
                break;
        }

    }

    Neg::Neg(const Register &reg) {
        setRegister(reg);
    }

    size_t Neg::getOpcodeSize() const {
        return 1;
    }

    std::vector<std::shared_ptr<Instruction>> Neg::neg(const Register &reg) {
        auto neg = std::make_shared<Neg>();
        neg->setRegister(reg);
        return {neg};
    }

    void Neg::dumpOpcode(std::vector<uint8_t> &data) const {
        if (opcode == 0)
            throw exception::ASMException("Invalid Neg opcode, did you see some values ?");
        data.push_back(opcode);
    }

    void Neg::dumpOther(std::vector<uint8_t> &data) const {
        (void)data;
    }

    Neg &Neg::setRegister(const Register &reg) {
        _register = reg;
        update();
        return *this;
    }

}
