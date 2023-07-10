/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Ret
*/

#pragma once

#include "asm/Instruction.hpp"

namespace raven::ASM {

    class Ret : public Instruction {
        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            Ret() = default;
            ~Ret() = default;

    };

}
