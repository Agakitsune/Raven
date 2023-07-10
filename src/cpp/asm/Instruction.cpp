/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Instruction
*/

#include <iostream>

#include "asm/Instruction.hpp"

namespace raven::ASM {

    void Instruction::clearLegacy() {
        _legacy.fill(0x00);
    }

    void Instruction::setLegacy(const Legacy &legacy) {
        _legacy.at(legacy.grp) = legacy.byte;
    }

    void Instruction::setRM(const ModRM &rm) {
        _rm = rm;
    }

    void Instruction::setREX(const REX &rex) {
        _rex = rex;
    }

    void Instruction::setSIB(const SIB &sib) {
        _sib = sib;
    }

    size_t Instruction::getSize() const {
        size_t size = 0;
        for (const auto &byte : _legacy) {
            if (byte)
                ++size;
        }
        if (_rex.has_value())
            ++size;
        size += getOpcodeSize();
        if (_rm.has_value())
            ++size;
        if (_sib.has_value())
            ++size;
        return size;
    }

    void Instruction::dump(std::vector<uint8_t> &data) const {
        for (const auto &byte : _legacy) {
            if (byte)
                data.push_back(byte);
        }
        if (_rex.has_value()) {
            data.push_back(_rex->getByte());
        }
        dumpOpcode(data);
        if (_rm.has_value())
            data.push_back(_rm->getByte());
        if (_sib.has_value())
            data.push_back(_sib->getByte());
        dumpOther(data);
    }

    void Instruction::dump(std::vector<uint8_t> &data, std::vector<std::shared_ptr<Instruction>> &code) {
        for (auto &ptr : code) {
            ptr->dump(data);
        }
    }

}
