/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Set
*/

#pragma once

#include <variant>

#include "asm/Instruction.hpp"
#include "asm/Register.hpp"
#include "asm/Immediate.hpp"

#include "asm/Jmp.hpp"

namespace raven::ASM {

    class Set : public Instruction {
        Register _value;
        Condition _cond = None;

        uint8_t opcode = 0x0;
        uint8_t opcode2 = 0x0;

        void update();

        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            Set() = default;
            Set(const Register &value, const Condition &condition = None);
            ~Set() = default;

            Set &setValue(const Register &value);
            Set &setCondition(const Condition &condition);

    };

}
