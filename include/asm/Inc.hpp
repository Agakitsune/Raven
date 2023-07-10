/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Inc
*/

#pragma once

#include "asm/Instruction.hpp"
#include "asm/Register.hpp"
#include "asm/Immediate.hpp"

namespace raven::ASM {

    class Inc : public Instruction {
        Register _reg;

        uint8_t opcode = 0x00;

        void update();

        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            Inc() = default;
            Inc(const Register &reg);
            ~Inc() = default;

            Inc &setReg(const Register &reg);

            static std::vector<std::shared_ptr<Instruction>> inc(const Register &reg);

    };

}
