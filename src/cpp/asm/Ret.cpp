/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Ret
*/

#include "asm/Ret.hpp"

namespace raven::ASM {

    size_t Ret::getOpcodeSize() const {
        return 1;
    }

    void Ret::dumpOpcode(std::vector<uint8_t> &data) const {
        data.push_back(0xc3);
    }

    void Ret::dumpOther(std::vector<uint8_t> &data) const {
        (void)data;
    }

}
