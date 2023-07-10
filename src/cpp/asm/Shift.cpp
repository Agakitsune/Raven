/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Shift
*/

#include <iostream>

#include "asm/Shift.hpp"
#include "asm/Mov.hpp"

namespace raven::ASM {

    void Shift::update() {

        clearLegacy();

        if (_src.index() == 0) {
            Register &r = std::get<Register>(_src);
            if (r != CL)
                throw exception::ASMException("Invalid Shift register, did you see some values ?");
            switch (_type) {
                case Right:
                    switch (_dst.size) {
                        case S8:
                            opcode = 0xd2;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(EDI);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S16:
                            opcode = 0xd3;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(EDI);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S32:
                            opcode = 0xd3;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(EDI);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S64:
                            opcode = 0xd3;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(EDI);
                            _rex = REX().W(true).B(_dst.extend);
                            break;
                    }
                    break;
                case Left:
                    switch (_dst.size) {
                        case S8:
                            opcode = 0xd2;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(ESP);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S16:
                            opcode = 0xd3;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(ESP);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S32:
                            opcode = 0xd3;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(ESP);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S64:
                            opcode = 0xd3;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(ESP);
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
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(EDI);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S16:
                            opcode = 0xc1;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(EDI);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S32:
                            opcode = 0xc1;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(EDI);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S64:
                            opcode = 0xc1;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(EDI);
                            _rex = REX().W(true).B(_dst.extend);
                            break;
                    }
                    break;
                case Left:
                    switch (_dst.size) {
                        case S8:
                            opcode = 0xc0;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(ESP);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S16:
                            opcode = 0xc1;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(ESP);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S32:
                            opcode = 0xc1;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(ESP);
                            if (_dst.useREX)
                                _rex = REX().B(_dst.extend);
                            else
                                _rex = std::nullopt;
                            break;
                        case S64:
                            opcode = 0xc1;
                            _rm = ModRM().setMod(Mod::Mod03).setRm(_dst).setReg(ESP);
                            _rex = REX().W(true).B(_dst.extend);
                            break;
                    }
                    break;
            }
        }

    }

    Shift &Shift::setDst(const Register &dst) {
        _dst = dst;
        update();
        return *this;
    }

    Shift &Shift::setType(const ShiftType &type) {
        _type = type;
        update();
        return *this;
    }

    std::vector<std::shared_ptr<Instruction>> Shift::sal(const Register &dst, const Register &src) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;

        auto mov = Move::safeMove(CL, src);
        instructions.insert(instructions.end(), mov.begin(), mov.end());

        auto shift = std::make_shared<ASM::Shift>();
        shift->setDst(dst).setSrc(CL).setType(ShiftType::Left);
        instructions.push_back(shift);

        return instructions;
    }

    std::vector<std::shared_ptr<Instruction>> Shift::sal(const Register &dst, const int8_t n) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;

        auto shift = std::make_shared<ASM::Shift>();
        shift->setDst(dst).setSrc(Immediate(n)).setType(ShiftType::Left);
        instructions.push_back(shift);

        return instructions;
    }

    std::vector<std::shared_ptr<Instruction>> Shift::sar(const Register &dst, const Register &src) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;

        auto mov = Move::safeMove(CL, src);
        instructions.insert(instructions.end(), mov.begin(), mov.end());

        auto shift = std::make_shared<ASM::Shift>();
        shift->setDst(dst).setSrc(CL).setType(ShiftType::Right);
        instructions.push_back(shift);

        return instructions;
    }

    std::vector<std::shared_ptr<Instruction>> Shift::sar(const Register &dst, const int8_t n) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;

        auto shift = std::make_shared<ASM::Shift>();
        shift->setDst(dst).setSrc(Immediate(n)).setType(ShiftType::Right);
        instructions.push_back(shift);

        return instructions;
    }

    size_t Shift::getOpcodeSize() const {
        if (_src.index() == 1)
            return 1 + std::get<Immediate>(_src).getImmediateSize();
        return 1;
    }

    void Shift::dumpOpcode(std::vector<uint8_t> &data) const {
        if (opcode == 255)
            throw exception::ASMException("Invalid Shift opcode, did you see some values ?");
        data.push_back(opcode);
    }

    void Shift::dumpOther(std::vector<uint8_t> &data) const {
        if (_src.index() == 1)
            std::get<Immediate>(_src).dump(data);
    }

}
