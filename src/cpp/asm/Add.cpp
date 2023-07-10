/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Add
*/

#include <iostream>

#include "asm/Add.hpp"

namespace raven::ASM {

    void Add::update() {

        clearLegacy();

        if (_src.index() == 1) {
            Immediate &imm = std::get<Immediate>(_src);
            if (_dst.reg == 0 && !_dst.extend) {
                switch (_dst.size) {
                    case S8:
                        opcode = 0x04;
                        imm.setSize(IMM8);
                        break;
                    case S16:
                        opcode = 0x05;
                        imm.setSize(IMM16);
                        setLegacy(OPOverride);
                        break;
                    case S32:
                        opcode = 0x05;
                        imm.setSize(IMM32);
                        break;
                    case S64:
                        opcode = 0x05;
                        imm.setSize(IMM32);
                        _rex = REX().W(true);
                        break;
                }
            } else {
                switch (_dst.size) {
                    case S8:
                        opcode = 0x80;
                        imm.setSize(IMM8);
                        _rm = ModRM().setMod(Mod::Mod03).setRm(_dst);
                        if (_dst.useREX)
                            _rex = REX().B(_dst.extend);
                        else
                            _rex = std::nullopt;
                        break;
                    case S16:
                        if (imm.getImmediateSize() == IMM8) {
                            opcode = 0x83;
                        } else {
                            opcode = 0x81;
                            imm.setSize(IMM16);
                        }
                        setLegacy(OPOverride);
                        _rm = ModRM().setMod(Mod::Mod03).setRm(_dst);
                        break;
                    case S32:
                        if (imm.getImmediateSize() == IMM8) {
                            opcode = 0x83;
                        } else {
                            opcode = 0x81;
                            imm.setSize(IMM32);
                        }
                        if (_dst.useREX)
                            _rex = REX().B(_dst.extend);
                        else
                            _rex = std::nullopt;
                        _rm = ModRM().setMod(Mod::Mod03).setRm(_dst);
                        break;
                    case S64:
                        if (imm.getImmediateSize() == IMM8) {
                            opcode = 0x83;
                        } else {
                            opcode = 0x81;
                            imm.setSize(IMM32);
                        }
                        _rex = REX().W(true).B(_dst.extend);
                        _rm = ModRM().setMod(Mod::Mod03).setRm(_dst);
                        break;
                }
            }
        } else {
            Register &r = std::get<Register>(_src);
            switch (_dst.size) {
                case S8:
                    opcode = 0x00;
                    if (r.size != S8)
                        throw exception::ASMException("Invalid size for register");
                    _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(r);
                    if (!_dst.useREX && _dst.reg > 3)
                        throw exception::ASMException("Cannot encode register, instruction is using REX byte");
                    if (r.useREX || _dst.useREX)
                        _rex = REX();
                    else
                        _rex = std::nullopt;
                    break;
                case S16:
                    setLegacy(OPOverride);
                case S32:
                    opcode = 0x01;
                    _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(r);
                    if (_dst.useREX || r.useREX)
                        _rex = REX().B(_dst.extend).R(r.extend);
                    else
                        _rex = std::nullopt;
                    break;
                case S64:
                    opcode = 0x01;
                    _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(r);
                    _rex = REX().W(true).B(_dst.extend).R(r.extend);
                    break;
            }
        }

    }

    Add &Add::setDst(const Register &dst) {
        _dst = dst;
        update();
        return *this;
    }

    std::vector<std::shared_ptr<Instruction>> Add::add(const Register &dst, const Register &src) {
        std::vector<std::shared_ptr<Instruction>> ret;
        auto add = std::make_shared<Add>();
        add->setDst(dst).setSrc(src);
        ret.push_back(add);
        return ret;
    }

    std::vector<std::shared_ptr<Instruction>> Add::add(const Register &dst, const uint64_t n) {
        std::vector<std::shared_ptr<Instruction>> ret;
        auto add = std::make_shared<Add>();
        add->setDst(dst).setSrc(Immediate(n));
        ret.push_back(add);
        return ret;
    }

    size_t Add::getOpcodeSize() const {
        if (_src.index() == 1)
            return 1 + std::get<Immediate>(_src).getImmediateSize();
        return 1;
    }

    void Add::dumpOpcode(std::vector<uint8_t> &data) const {
        if (opcode == 255)
            throw exception::ASMException("Invalid ADD opcode, did you see some values ?");
        data.push_back(opcode);
    }

    void Add::dumpOther(std::vector<uint8_t> &data) const {
        if (_src.index() == 1)
            std::get<Immediate>(_src).dump(data);
    }

}
