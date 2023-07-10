/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Move
*/

#include <iostream>

#include "asm/Move.hpp"

namespace raven::ASM {

    void Move::update() {

        clearLegacy();

        D = false;

        switch (_type) {
            case MoveType::SignExtend:
                switch (_src.size) {
                    case S8:
                        opcode = 0xbe;
                        switch (_dst.size) {
                            case S8:
                                throw std::runtime_error("Sign extend not supported for this size");
                            case S16:
                                setLegacy(OPOverride);
                            case S32:
                                _rm = ModRM().setMod(Mod::Mod03).setRm(_src).setReg(_dst);
                                if (_src.useREX || _dst.useREX)
                                    _rex = REX().B(_src.extend).R(_dst.extend);
                                else
                                    _rex = std::nullopt;
                                break;
                            case S64:
                                _rm = ModRM().setMod(Mod::Mod03).setRm(_src).setReg(_dst);
                                _rex = REX().W(true).R(_dst.extend).B(_src.extend);
                        }
                        break;
                    case S16:
                        opcode = 0xbf;
                        switch (_dst.size) {
                            case S8:
                            case S16:
                                throw std::runtime_error("Sign extend not supported for this size");
                            case S32:
                                _rm = ModRM().setMod(Mod::Mod03).setRm(_src).setReg(_dst);
                                if (_src.useREX || _dst.useREX)
                                    _rex = REX().B(_src.extend).R(_dst.extend);
                                else
                                    _rex = std::nullopt;
                                break;
                            case S64:
                                _rm = ModRM().setMod(Mod::Mod03).setRm(_src).setReg(_dst);
                                _rex = REX().W(true).R(_dst.extend).B(_src.extend);
                        }
                        break;
                    case S32:
                        D = true;
                        opcode = 0x63;
                        switch (_dst.size) {
                            case S8:
                            case S16:
                                throw std::runtime_error("Sign extend not supported for this size");
                            case S32:
                                _rm = ModRM().setMod(Mod::Mod03).setRm(_src).setReg(_dst);
                                if (_src.useREX || _dst.useREX)
                                    _rex = REX().B(_src.extend).R(_dst.extend);
                                else
                                    _rex = std::nullopt;
                                break;
                            case S64:
                                _rm = ModRM().setMod(Mod::Mod03).setRm(_src).setReg(_dst);
                                _rex = REX().W(true).R(_dst.extend).B(_src.extend);
                        }
                        break;
                    case S64:
                        throw std::runtime_error("Sign extend not supported for this size");
                }
                break;
            case MoveType::ZeroExtend:
                switch (_src.size) {
                    case S8:
                        opcode = 0xb6;
                        switch (_dst.size) {
                            case S8:
                                throw std::runtime_error("Zero extend not supported for this size");
                            case S16:
                                setLegacy(OPOverride);
                            case S32:
                                _rm = ModRM().setMod(Mod::Mod03).setRm(_src).setReg(_dst);
                                if (_src.useREX || _dst.useREX)
                                    _rex = REX().B(_src.extend).R(_dst.extend);
                                else
                                    _rex = std::nullopt;
                                break;
                            case S64:
                                _rm = ModRM().setMod(Mod::Mod03).setRm(_src).setReg(_dst);
                                _rex = REX().W(true).R(_dst.extend).B(_src.extend);
                        }
                        break;
                    case S16:
                        opcode = 0xb7;
                        switch (_dst.size) {
                            case S8:
                            case S16:
                                throw std::runtime_error("Zero extend not supported for this size");
                            case S32:
                                _rm = ModRM().setMod(Mod::Mod03).setRm(_src).setReg(_dst);
                                if (_src.useREX || _dst.useREX)
                                    _rex = REX().B(_src.extend).R(_dst.extend);
                                else
                                    _rex = std::nullopt;
                                break;
                            case S64:
                                _rm = ModRM().setMod(Mod::Mod03).setRm(_src).setReg(_dst);
                                _rex = REX().W(true).R(_dst.extend).B(_src.extend);
                        }
                        break;
                    case S32:
                    case S64:
                        throw std::runtime_error("Zero extend not supported for this size");
                }
                break;
        }

    }

    size_t Move::getOpcodeSize() const {
        if (D)
            return 1;
        return 2;
    }

    void Move::dumpOpcode(std::vector<uint8_t> &data) const {
        if (!D)
            data.push_back(0x0f);
        data.push_back(opcode);
    }

    void Move::dumpOther(std::vector<uint8_t> &data) const {
    }

    Move::Move(const Register &dst, const Register &src, const MoveType &type) : _dst(dst), _src(src), _type(type) {
        update();
    }

    Move &Move::setDst(const Register &dst) {
        _dst = dst;
        update();
        return *this;
    }

    Move &Move::setSrc(const Register &src) {
        _src = src;
        update();
        return *this;
    }

    Move &Move::setType(const MoveType &type) {
        _type = type;
        update();
        return *this;
    }

    std::vector<std::shared_ptr<Instruction>> Move::move(const Register &dst, const Register &src, const MoveType &type) {
        auto move = std::make_shared<Move>(dst, src, type);
        return {move};
    }

    std::vector<std::shared_ptr<Instruction>> Move::safeMove(const Register &dst, const Register &src,  const MoveType &type) {
        std::vector<std::shared_ptr<Instruction>> instructions;

        Register d = dst;
        Register s = src;

        switch (dst.size) {
            case S8:
                if (src.size != S8)
                    d.size = src.size;
                instructions.push_back(std::make_shared<Mov>(d, s));
                break;
            case S16:
                if (src.size == S8)
                    instructions.push_back(std::make_shared<Move>(d, s, type));
                else if (src.size > S16)
                    d.size = src.size;
                instructions.push_back(std::make_shared<Mov>(d, s));
                break;
            case S32:
                if (src.size < S32)
                    instructions.push_back(std::make_shared<Move>(d, s, type));
                else if (src.size > S32)
                    d.size = src.size;
                instructions.push_back(std::make_shared<Mov>(d, s));
                break;
            case S64:
                if (src.size < S64)
                    instructions.push_back(std::make_shared<Move>(d, s, type));
                else
                    instructions.push_back(std::make_shared<Mov>(d, s));
                break;
        }

        return instructions;
    }

}
