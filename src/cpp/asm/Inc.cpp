/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Inc
*/

#include "asm/Inc.hpp"

namespace raven::ASM {

    void Inc::update() {

        clearLegacy();

        switch (_reg.size) {
            case S8:
                opcode = 0xfe;
                _rm = ModRM().setMod(Mod::Mod03).setRm(_reg);
                if (_reg.useREX)
                    _rex = REX();
                else
                    _rex = std::nullopt;
                break;
            case S16:
                setLegacy(OPOverride);
            case S32:
                opcode = 0xff;
                _rm = ModRM().setMod(Mod::Mod03).setRm(_reg);
                if (_reg.useREX)
                    _rex = REX().B(_reg.extend);
                else
                    _rex = std::nullopt;
                break;
            case S64:
                opcode = 0xff;
                _rm = ModRM().setMod(Mod::Mod03).setRm(_reg);
                _rex = REX().W(true).B(_reg.extend);
                break;
        }

    }

    Inc::Inc(const Register &reg) {
        setReg(reg);
    }

    Inc &Inc::setReg(const Register &reg) {
        _reg = reg;
        update();
        return *this;
    }

    std::vector<std::shared_ptr<Instruction>> Inc::inc(const Register &reg) {
        auto inc = std::make_shared<Inc>();
        inc->setReg(reg);
        return {inc};
    }

    size_t Inc::getOpcodeSize() const {
        return 1;
    }

    void Inc::dumpOpcode(std::vector<uint8_t> &data) const {
        if (opcode == 0)
            throw exception::ASMException("Invalid ADD opcode, did you see some values ?");
        data.push_back(opcode);
    }

    void Inc::dumpOther(std::vector<uint8_t> &data) const {
        (void)data;
    }

}
