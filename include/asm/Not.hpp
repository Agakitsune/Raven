/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Not
*/

#pragma once

#include "asm/Instruction.hpp"
#include "asm/Register.hpp"

namespace raven::ASM {

    class Not : public Instruction {
        Register _register;

        uint8_t opcode = 0x00;

        void update();

        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            Not() = default;
            Not(const Register &reg);
            ~Not() = default;

            Not &setRegister(const Register &reg);

            static std::vector<std::shared_ptr<Instruction>> not_(const Register &reg);

    };

}
