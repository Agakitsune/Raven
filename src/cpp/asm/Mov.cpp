/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Mov
*/

#include <iostream>

#include "asm/Mov.hpp"

namespace raven::ASM {

    void Mov::update() {

        clearLegacy();

        if (_src.index() == 2) {
            Access &a = std::get<Access>(_src);
            if (_dst.index() == 1) {
                throw exception::ASMException("Too many memory access");
            } else {
                Register &rd = std::get<Register>(_dst);
                if (a.getValue().index() == 0) {
                    const Register &ar = std::get<Register>(a.getValue());
                    if (rd.size == S16)
                        setLegacy(OPOverride);
                    switch (ar.size) {
                        case S8:
                        case S16:
                            std::cout << "oh dang it" << std::endl;
                            throw exception::ASMException("Invalid register size");
                        case S32:
                            setLegacy(ADDROverride);
                        case S64:
                            opcode = rd.size == S8 ? 0x8A : 0x8B;
                            if (a.getOffset().index() == 2) {
                                Immediate ai = std::get<Immediate>(a.getOffset());
                                switch (ai.getImmediateSize()) {
                                    case IMM8:
                                        if (rd.size == S64)
                                            _rex = REX().W(true).B(ar.extend).R(rd.extend);
                                        else if (rd.useREX)
                                            _rex = REX().B(ar.extend).R(rd.extend);
                                        else
                                            _rex = std::nullopt;
                                        _rm = ModRM().setMod(Mod::Mod01).setRm(ar).setReg(rd);
                                        break;
                                    case IMM16:
                                    case IMM32:
                                    case IMM64:
                                        ai.setSize(IMM32);
                                        a.setOffset(ai);
                                        if (rd.size == S64)
                                            _rex = REX().W(true).B(ar.extend).R(rd.extend);
                                        else if (rd.useREX)
                                            _rex = REX().B(ar.extend).R(rd.extend);
                                        else
                                            _rex = std::nullopt;
                                        _rm = ModRM().setMod(Mod::Mod01).setRm(ar).setReg(rd);
                                        break;
                                }
                            } else if (a.getOffset().index() == 0) {
                                _rm = ModRM().setMod(Mod::Mod00).setRm(ar).setReg(rd);
                                if (ar.useREX || rd.useREX)
                                    _rex = REX().W(true).B(ar.extend).R(rd.extend);
                                else
                                    _rex = std::nullopt;
                            } else if (a.getOffset().index() == 1) {
                                const Register &ir = std::get<Register>(a.getOffset());
                                _rm = ModRM().setMod(Mod::Mod00).setRm(ASM::RSP).setReg(rd);
                                _sib = SIB().scale(static_cast<uint8_t>(a.getAccessFactor())).base(ar.reg).index(ir.reg);
                                _rex = REX().W(rd.size == S64).R(rd.extend).X(ir.extend).B(ar.extend);
                            } else {
                                throw exception::ASMException("Not supported");
                            }
                            break;
                    }
                } else {
                    Immediate ai = std::get<Immediate>(a.getValue());
                    opcode = rd.size == S8 ? 0x8A : 0x8B;
                    if (rd.useREX || rd.size == S64)
                        _rex = REX().W(true).R(rd.extend);
                    else
                        _rex = std::nullopt;
                    _rm = ModRM().setMod(Mod::Mod00).setRm(ESP).setReg(rd);
                    _sib = SIB().scale(0b00).base(0b101).index(0b100);
                    ai.setSize(IMM32);
                                        a.setValue(ai);
                                    }
            }
        } else if (_src.index() == 1) {
            Immediate &i = std::get<Immediate>(_src);
            if (_dst.index() == 0) {
                Register &rd = std::get<Register>(_dst);
                switch (rd.size) {
                    case S8:
                        opcode = 0xB0 + rd.reg;
                        if (rd.useREX)
                            _rex = REX().B(rd.extend);
                        else
                            _rex = std::nullopt;
                        i.setSize(IMM8);
                        break;
                    case S16:
                        i.setSize(IMM16);
                        setLegacy(OPOverride);
                    case S32:
                        opcode = 0xB8 + rd.reg;
                        if (rd.useREX)
                            _rex = REX().B(rd.extend);
                        else
                            _rex = std::nullopt;
                        i.setSize(IMM32);
                        break;
                    case S64:
                        opcode = 0xC7;
                        _rex = REX().W(true).B(rd.extend);
                        _rm = ModRM().setMod(Mod::Mod03).setRm(rd);
                        i.setSize(IMM32);
                        break;
                }
            }
        } else {
            Register &r = std::get<Register>(_src);
            if (_dst.index() == 0) {
                Register &rd = std::get<Register>(_dst);
                if (rd.size != r.size) {
                    std::cout << "oh crap" << std::endl;
                    throw exception::ASMException("Invalid register size");
                }
                switch (r.size) {
                    case S8:
                        opcode = 0x88;
                        if (rd.useREX || r.useREX)
                            _rex = REX().B(rd.extend).R(r.extend);
                        else
                            _rex = std::nullopt;
                        _rm = ModRM().setMod(Mod::Mod03).setRm(rd).setReg(r);
                        break;
                    case S16:
                        opcode = 0x88;
                        if (rd.useREX || r.useREX)
                            _rex = REX().B(rd.extend).R(r.extend);
                        else
                            _rex = std::nullopt;
                        setLegacy(OPOverride);
                        _rm = ModRM().setMod(Mod::Mod03).setRm(rd).setReg(r);
                        break;
                    case S32:
                        opcode = 0x89;
                        if (rd.useREX || r.useREX)
                            _rex = REX().B(rd.extend).R(r.extend);
                        else
                            _rex = std::nullopt;
                        _rm = ModRM().setMod(Mod::Mod03).setRm(rd).setReg(r);
                        break;
                    case S64:
                                                opcode = 0x89;
                        _rex = REX().W(true).B(rd.extend).R(r.extend);
                        _rm = ModRM().setMod(Mod::Mod03).setRm(rd).setReg(r);
                        break;
                }
            } else {
                Access &a = std::get<Access>(_dst);
                if (r.size == S16)
                    setLegacy(OPOverride);
                if (a.getValue().index() == 0) {
                    const Register &ar = std::get<Register>(a.getValue());
                    switch (ar.size) {
                        case S8:
                        case S16:
                            std::cout << "oh shit" << std::endl;
                            throw exception::ASMException("Invalid register size");
                        case S32:
                            setLegacy(ADDROverride);
                        case S64:
                            opcode = 0x89;
                            if (a.getOffset().index() == 2) {
                                Immediate ai = std::get<Immediate>(a.getOffset());
                                switch (ai.getImmediateSize()) {
                                    case IMM8:
                                        if (r.size == S64)
                                            _rex = REX().W(true).B(ar.extend).R(r.extend);
                                        else if (r.useREX)
                                            _rex = REX().B(ar.extend).R(r.extend);
                                        else
                                            _rex = std::nullopt;
                                        _rm = ModRM().setMod(Mod::Mod01).setRm(ar).setReg(r);
                                        break;
                                    case IMM16:
                                    case IMM32:
                                    case IMM64:
                                        ai.setSize(IMM32);
                                        a.setOffset(ai);
                                        if (r.size == S64)
                                            _rex = REX().W(true).B(ar.extend).R(r.extend);
                                        else if (r.useREX)
                                            _rex = REX().B(ar.extend).R(r.extend);
                                        else
                                            _rex = std::nullopt;
                                        _rm = ModRM().setMod(Mod::Mod01).setRm(ar).setReg(r);
                                        break;
                                }
                            } else if (a.getOffset().index() == 0) {
                                _rm = ModRM().setMod(Mod::Mod00).setRm(ar).setReg(r);
                                if (ar.useREX || r.useREX)
                                    _rex = REX().W(true).B(ar.extend).R(r.extend);
                                else
                                    _rex = std::nullopt;
                            } else {
                                throw exception::ASMException("Not supported");
                            }
                            break;
                    }
                } else {
                    const Immediate &ai = std::get<Immediate>(a.getValue());
                    opcode = 0x89;
                    if (r.useREX)
                        _rex = REX().W(true).R(r.extend);
                    else
                        _rex = std::nullopt;
                    _rm = ModRM().setMod(Mod::Mod00).setRm(ESP).setReg(r);
                    _sib = SIB().scale(0b100).base(0b101).scale(0);
                }
            }
        }

    }

    size_t Mov::getOpcodeSize() const {
        if (_dst.index() == 1) {
            const Access &a = std::get<Access>(_dst);
            if (a.getValue().index() == 1) {
                const Immediate &ai = std::get<Immediate>(a.getValue());
                return 1 + ai.getImmediateSize();
            } else {
                if (a.getOffset().index() == 2) {
                    const Immediate &ai = std::get<Immediate>(a.getOffset());
                    return 1 + ai.getImmediateSize();
                }
            }
            return 1;
        } else if (_src.index() == 1) {
            return 1 + std::get<Immediate>(_src).getImmediateSize();
        } else {
            if (_src.index() == 2) {
                const Access &a = std::get<Access>(_src);
                if (a.getValue().index() == 1) {
                    const Immediate &ai = std::get<Immediate>(a.getValue());
                                        return 1 + ai.getImmediateSize();
                } else {
                    if (a.getOffset().index() == 2) {
                        const Immediate &ai = std::get<Immediate>(a.getOffset());
                        return 1 + ai.getImmediateSize();
                    }
                }
            }
            return 1;
        }
    }

    void Mov::dumpOpcode(std::vector<uint8_t> &data) const {
        data.push_back(opcode);
    }

    void Mov::dumpOther(std::vector<uint8_t> &data) const {
        if (_src.index() == 2) {
            const Access &a = std::get<Access>(_src);
            if (a.getValue().index() == 1) {
                const Immediate &ai = std::get<Immediate>(a.getValue());
                                ai.dump(data);
            } else {
                if (a.getOffset().index() == 2) {
                    const Immediate &ai = std::get<Immediate>(a.getOffset());
                    ai.dump(data);
                }
            }
        } else if (_src.index() == 1) {
            std::get<Immediate>(_src).dump(data);
        } else {
            if (_dst.index() == 1) {
                const Access &a = std::get<Access>(_dst);
                if (a.getValue().index() == 1) {
                    const Immediate &ai = std::get<Immediate>(a.getValue());
                    ai.dump(data);
                } else {
                    if (a.getOffset().index() == 2) {
                        const Immediate &ai = std::get<Immediate>(a.getOffset());
                        ai.dump(data);
                    }
                }
            }
        }
    }

}
