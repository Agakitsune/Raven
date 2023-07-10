/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** IDiv
*/

#pragma once

#include "asm/Instruction.hpp"
#include "asm/Register.hpp"

#include "asm/Mov.hpp"
#include "asm/Xor.hpp"
#include "asm/Move.hpp"

namespace raven::ASM {

    class IDiv : public Instruction {
        Register _register;

        uint8_t opcode = 0x00;

        void update();

        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            IDiv() = default;
            ~IDiv() = default;

            IDiv &setRegister(const Register &reg);

            static std::vector<std::shared_ptr<Instruction>> div(const Register &dst, const Register &src);
            static std::vector<std::shared_ptr<Instruction>> div(const Register &dst, const int64_t &src);
            static std::vector<std::shared_ptr<Instruction>> mod(const Register &dst, const Register &src);
            static std::vector<std::shared_ptr<Instruction>> mod(const Register &dst, const int64_t &src);

    };

}
