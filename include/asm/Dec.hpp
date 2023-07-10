/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Dec
*/

#pragma once

#include "asm/Instruction.hpp"
#include "asm/Register.hpp"
#include "asm/Immediate.hpp"

namespace raven::ASM {

    class Dec : public Instruction {
        Register _reg;

        uint8_t opcode = 0x00;

        void update();

        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            Dec() = default;
            Dec(const Register &reg);
            ~Dec() = default;

            Dec &setReg(const Register &reg);

            static std::vector<std::shared_ptr<Instruction>> dec(const Register &reg);

    };

}
