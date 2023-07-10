/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Pop
*/

#pragma once

#include "asm/Instruction.hpp"
#include "asm/Register.hpp"

namespace raven::ASM {

    class Pop : public Instruction {
        Register _register;

        uint8_t opcode = 0x00;

        void update();

        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            Pop() = default;
            Pop(const Register &reg);
            ~Pop() = default;

            Pop &setRegister(const Register &reg);

    };

}
