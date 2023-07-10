/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Not
*/

#include "asm/Not.hpp"

namespace raven::ASM {

    void Not::update() {

        clearLegacy();

        switch (_register.size) {
            case S8:
                opcode = 0xf6;
                _rm = ModRM().setMod(Mod::Mod03).setRm(_register).setReg(EDX);
                if (_register.useREX)
                    _rex = REX().B(_register.extend);
                else
                    _rex = std::nullopt;
                break;
            case S16:
                setLegacy(OPOverride);
            case S32:
                opcode = 0xf7;
                _rm = ModRM().setMod(Mod::Mod03).setRm(_register).setReg(EDX);
                if (_register.useREX)
                    _rex = REX().B(_register.extend);
                else
                    _rex = std::nullopt;
                break;
            case S64:
                opcode = 0xf7;
                _rm = ModRM().setMod(Mod::Mod03).setRm(_register).setReg(EDX);
                _rex = REX().W(true).B(_register.extend);
                break;
        }

    }

    Not::Not(const Register &reg) {
        setRegister(reg);
    }

    size_t Not::getOpcodeSize() const {
        return 1;
    }

    std::vector<std::shared_ptr<Instruction>> Not::not_(const Register &reg) {
        auto not_ = std::make_shared<Not>();
        not_->setRegister(reg);
        return {not_};
    }

    void Not::dumpOpcode(std::vector<uint8_t> &data) const {
        if (opcode == 0)
            throw exception::ASMException("Invalid Not opcode, did you see some values ?");
        data.push_back(opcode);
    }

    void Not::dumpOther(std::vector<uint8_t> &data) const {
        (void)data;
    }

    Not &Not::setRegister(const Register &reg) {
        _register = reg;
        update();
        return *this;
    }

}
