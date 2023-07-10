/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** IMul
*/

#include <iostream>

#include "asm/IMul.hpp"

namespace raven::ASM {

    void IMul::update() {

        clearLegacy();
        opcode2 = 0xff;

        if (_src.index() == 1) {
            Immediate &imm = std::get<Immediate>(_src);
            switch (_dst.size) {
                case S8:
                    throw exception::ASMException("Invalid size for register");
                case S16:
                    setLegacy(OPOverride);
                    if (imm.getImmediateSize() == IMM8) {
                        opcode = 0x6B;
                    } else {
                        opcode = 0x69;
                        imm.setSize(IMM16);
                    }
                    if (_dst.useREX)
                        _rex = REX().B(_dst.extend).R(_dst.extend);
                    else
                        _rex = std::nullopt;
                    _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(_dst);
                    break;
                case S32:
                    if (imm.getImmediateSize() == IMM8) {
                        opcode = 0x6B;
                    } else {
                        opcode = 0x69;
                        imm.setSize(IMM32);
                    }
                    if (_dst.useREX)
                        _rex = REX().B(_dst.extend).R(_dst.extend);
                    else
                        _rex = std::nullopt;
                    _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(_dst);
                    break;
                case S64:
                    if (imm.getImmediateSize() == IMM8) {
                        opcode = 0x6B;
                    } else {
                        opcode = 0x69;
                        imm.setSize(IMM32);
                    }
                    _rex = REX().W(true).B(_dst.extend).R(_dst.extend);
                    _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(_dst);
                    break;
            }
        } else {
            Register &r = std::get<Register>(_src);
            opcode = 0x0f;
            opcode2 = 0xaf;
            switch (_dst.size) {
                case S8:
                    throw exception::ASMException("Invalid size for register");
                case S16:
                    setLegacy(OPOverride);
                    if (_dst.useREX || r.useREX)
                        _rex = REX().B(r.extend).R(_dst.extend);
                    else
                        _rex = std::nullopt;
                    _rm = ModRM().setMod(Mod::Mod03).setRm(r).setReg(_dst);
                    break;
                case S32:
                    if (_dst.useREX || r.useREX)
                        _rex = REX().B(r.extend).R(_dst.extend);
                    else
                        _rex = std::nullopt;
                    _rm = ModRM().setMod(Mod::Mod03).setRm(r).setReg(_dst);
                    break;
                case S64:
                    _rm = ModRM().setMod(Mod::Mod03).setRm(r).setReg(_dst);
                    _rex = REX().W(true).B(r.extend).R(_dst.extend);
                    break;
            }
        }

    }

    IMul &IMul::setDst(const Register &dst) {
        _dst = dst;
        update();
        return *this;
    }

    std::vector<std::shared_ptr<Instruction>> IMul::mul(const Register &dst, const Register &src) {
        auto mul = std::make_shared<IMul>();
        mul->setDst(dst);
        mul->setSrc(src);
        return {mul};
    }

    std::vector<std::shared_ptr<Instruction>> IMul::mul(const Register &dst, const int64_t &src) {
        auto mul = std::make_shared<IMul>();
        mul->setDst(dst);
        mul->setSrc(Immediate(src));
        return {mul};
    }

    size_t IMul::getOpcodeSize() const {
        size_t i = 1;
        if (opcode2 != 0xff)
            i++;
        if (_src.index() == 1)
            return i + std::get<Immediate>(_src).getImmediateSize();
        return i;
    }

    void IMul::dumpOpcode(std::vector<uint8_t> &data) const {
        if (opcode == 255)
            throw exception::ASMException("Invalid IMul opcode, did you see some values ?");
        data.push_back(opcode);
        if (opcode2 != 0xff)
            data.push_back(opcode2);
    }

    void IMul::dumpOther(std::vector<uint8_t> &data) const {
        if (_src.index() == 1)
            std::get<Immediate>(_src).dump(data);
    }

}
