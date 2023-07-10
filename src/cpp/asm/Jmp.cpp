/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Jmp
*/

#include <iostream>

#include "asm/Jmp.hpp"

namespace raven::ASM {

    Jmp::Jmp(const Condition &condition) : _cond(condition) {}

    void Jmp::update() {

        uint8_t code[] = {
            0x00, // unused
            0x77,
            0x73,
            0x72,
            0x76,
            0x72,
            0xe3,
            0xe3,
            0x74,
            0x7f,
            0x7d,
            0x7c,
            0x7e,
            0x76,
            0x72,
            0x73,
            0x77,
            0x73,
            0x75,
            0x7e,
            0x7c,
            0x7d,
            0x7f,
            0x71,
            0x7b,
            0x79,
            0x75,
            0x70,
            0x7a,
            0x7a,
            0x7b,
            0x78,
            0x74
        };

        opcode2 = 0;
        clearLegacy();

        if (_cond == None) {
            if (_value.index() == 1) {
                Immediate &i = std::get<Immediate>(_value);
                switch (i.getImmediateSize()) {
                    case IMM8:
                        opcode = 0xe8;
                        break;
                    case IMM16:
                        throw exception::ASMException("16 bit relative not supported in 64 bit mode");
                    case IMM64:
                        i.setSize(IMM32);
                    case IMM32:
                        opcode = 0xe9;
                }
            } else {
                Register &r = std::get<Register>(_value);
                switch (r.size) {
                    case S8:
                        throw exception::ASMException("not supported");
                    case S16:
                        throw exception::ASMException("not supported");
                    case S32:
                        setLegacy(ADDROverride);
                    case S64:
                        opcode = 0xff;
                        _rm = ModRM().setMod(Mod::Mod03).setReg(RSP).setRm(r);
                        if (r.useREX)
                            _rex = REX().B(r.extend);
                        else
                            _rex = std::nullopt;
                        break;
                }
            }
        } else {
            if (_value.index() == 1) {
                Immediate &i = std::get<Immediate>(_value);
                switch (i.getImmediateSize()) {
                    case IMM8:
                        opcode = code[static_cast<int>(_cond)];
                        if (_cond != ZeroECX)
                            setLegacy(ADDROverride);
                        break;
                    case IMM16:
                        i.setSize(IMM16);
                    case IMM32:
                        if (_cond == ZeroECX || _cond == ZeroRCX)
                            throw exception::ASMException("32 bit relative not supported with JECXZ or JRCXZ");
                        opcode = 0x0f;
                        opcode2 = code[static_cast<int>(_cond)] + 0x10;
                        break;
                    case IMM64:
                        throw exception::ASMException("64 bit relative not supported with Jcc in 64 bit mode");
                }
            } else {
                throw exception::ASMException("Invalid combination of opcode and operands");
            }
        }

    }

    size_t Jmp::getOpcodeSize() const {
        size_t size = 1;
        if (opcode2 != 0)
            size++;
        if (_value.index() == 1)
            return size + std::get<Immediate>(_value).getImmediateSize();
        return size;
    }

    void Jmp::dumpOpcode(std::vector<uint8_t> &data) const {
        if (opcode == 0)
            throw exception::ASMException("Invalid Jmp opcode, did you see some values ?");
        data.push_back(opcode);
        if (opcode2 != 0)
            data.push_back(opcode2);
    }

    void Jmp::dumpOther(std::vector<uint8_t> &data) const {
        if (_value.index() == 1)
            std::get<Immediate>(_value).dump(data);
    }

}
