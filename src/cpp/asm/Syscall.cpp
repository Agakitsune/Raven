/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Syscall
*/

#include "asm/Syscall.hpp"

namespace raven::ASM {

    size_t Syscall::getOpcodeSize() const {
        return 2;
    }

    void Syscall::dumpOpcode(std::vector<uint8_t> &data) const {
        data.push_back(0x0f);
        data.push_back(0x05);
    }

    void Syscall::dumpOther(std::vector<uint8_t> &data) const {
        (void)data;
    }

}
