/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Dec
*/

#include "asm/Dec.hpp"

namespace raven::ASM {

    void Dec::update() {

        clearLegacy();

        switch (_reg.size) {
            case S8:
                opcode = 0xfe;
                _rm = ModRM().setMod(Mod::Mod03).setRm(_reg).setReg(ECX);
                if (_reg.useREX)
                    _rex = REX();
                else
                    _rex = std::nullopt;
                break;
            case S16:
                setLegacy(OPOverride);
            case S32:
                opcode = 0xff;
                _rm = ModRM().setMod(Mod::Mod03).setRm(_reg).setReg(ECX);
                if (_reg.useREX)
                    _rex = REX().B(_reg.extend);
                else
                    _rex = std::nullopt;
                break;
            case S64:
                opcode = 0xff;
                _rm = ModRM().setMod(Mod::Mod03).setRm(_reg).setReg(ECX);
                _rex = REX().W(true).B(_reg.extend);
                break;
        }

    }

    Dec::Dec(const Register &reg) {
        setReg(reg);
    }

    Dec &Dec::setReg(const Register &reg) {
        _reg = reg;
        update();
        return *this;
    }

    std::vector<std::shared_ptr<Instruction>> Dec::dec(const Register &reg) {
        auto dec = std::make_shared<Dec>();
        dec->setReg(reg);
        return {dec};
    }

    size_t Dec::getOpcodeSize() const {
        return 1;
    }

    void Dec::dumpOpcode(std::vector<uint8_t> &data) const {
        if (opcode == 0)
            throw exception::ASMException("Invalid Dec opcode, did you see some values ?");
        data.push_back(opcode);
    }

    void Dec::dumpOther(std::vector<uint8_t> &data) const {
        (void)data;
    }

}
