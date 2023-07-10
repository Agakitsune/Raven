/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Rotate
*/

#include <iostream>

#include "asm/Rotate.hpp"

namespace raven::ASM {

    void Rotate::update() {

        clearLegacy();

        if (_src.index() == 0) {
            Register &r = std::get<Register>(_src);
            if (r != CL)
                throw exception::ASMException("Invalid Rotate register, did you see some values ?");
            switch (_type) {
                case Right:
                    switch (_dst.size) {
                        case S8:
                            opcode = 0xd2;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(ECX);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S16:
                            opcode = 0xd3;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(ECX);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S32:
                            opcode = 0xd3;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(ECX);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S64:
                            opcode = 0xd3;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(ECX);
                            _rex = REX().W(true).B(_dst.extend);
                            break;
                    }
                    break;
                case Left:
                    switch (_dst.size) {
                        case S8:
                            opcode = 0xd2;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(EAX);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S16:
                            opcode = 0xd3;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(EAX);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S32:
                            opcode = 0xd3;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(EAX);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S64:
                            opcode = 0xd3;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(EAX);
                            _rex = REX().W(true).B(_dst.extend);
                            break;
                    }
                    break;
            }
        } else {
            Immediate &i = std::get<Immediate>(_src);
            i.setSize(IMM8);
            switch (_type) {
                case Right:
                    switch (_dst.size) {
                        case S8:
                            opcode = 0xc0;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(ECX);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S16:
                            opcode = 0xc1;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(ECX);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S32:
                            opcode = 0xc1;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(ECX);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S64:
                            opcode = 0xc1;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(ECX);
                            _rex = REX().W(true).B(_dst.extend);
                            break;
                    }
                    break;
                case Left:
                    switch (_dst.size) {
                        case S8:
                            opcode = 0xc0;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(EAX);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S16:
                            opcode = 0xc1;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(EAX);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S32:
                            opcode = 0xc1;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(EAX);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S64:
                            opcode = 0xc1;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(EAX);
                            _rex = REX().W(true).B(_dst.extend);
                            break;
                    }
                    break;
            }
        }

    }

    Rotate &Rotate::setDst(const Register &dst) {
        _dst = dst;
        update();
        return *this;
    }

    Rotate &Rotate::setType(const ShiftType &type) {
        _type = type;
        update();
        return *this;
    }

    std::vector<std::shared_ptr<Instruction>> Rotate::rol(const Register &dst, const Register &src) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        Register r = RCX;

        r.size = src.size;

        auto mov = std::make_shared<ASM::Mov>();
        mov->setDst(r).setSrc(src);
        instructions.push_back(mov);

        auto rotate = std::make_shared<ASM::Rotate>();
        rotate->setDst(dst).setSrc(r).setType(ShiftType::Left);
        instructions.push_back(rotate);

        return instructions;
    }

    std::vector<std::shared_ptr<Instruction>> Rotate::rol(const Register &dst, const int8_t n) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;

        auto rotate = std::make_shared<ASM::Rotate>();
        rotate->setDst(dst).setSrc(Immediate(n)).setType(ShiftType::Left);
        instructions.push_back(rotate);

        return instructions;
    }

    std::vector<std::shared_ptr<Instruction>> Rotate::ror(const Register &dst, const Register &src) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        Register r = RCX;

        r.size = src.size;

        auto mov = std::make_shared<ASM::Mov>();
        mov->setDst(r).setSrc(src);
        instructions.push_back(mov);

        auto rotate = std::make_shared<ASM::Rotate>();
        rotate->setDst(dst).setSrc(r).setType(ShiftType::Right);
        instructions.push_back(rotate);

        return instructions;
    }

    std::vector<std::shared_ptr<Instruction>> Rotate::ror(const Register &dst, const int8_t n) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;

        auto rotate = std::make_shared<ASM::Rotate>();
        rotate->setDst(dst).setSrc(Immediate(n)).setType(ShiftType::Right);
        instructions.push_back(rotate);

        return instructions;
    }

    size_t Rotate::getOpcodeSize() const {
        if (_src.index() == 1)
            return 1 + std::get<Immediate>(_src).getImmediateSize();
        return 1;
    }

    void Rotate::dumpOpcode(std::vector<uint8_t> &data) const {
        if (opcode == 255)
            throw exception::ASMException("Invalid Rotate opcode, did you see some values ?");
        data.push_back(opcode);
    }

    void Rotate::dumpOther(std::vector<uint8_t> &data) const {
        if (_src.index() == 1)
            std::get<Immediate>(_src).dump(data);
    }

}
