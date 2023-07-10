/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Call
*/

#pragma once

#include <variant>

#include "asm/Instruction.hpp"
#include "asm/Register.hpp"
#include "asm/Immediate.hpp"

namespace raven::ASM {

    class Call : public Instruction {
        std::variant<Register, Immediate> _value = Immediate(0);

        uint8_t opcode = 0x00;

        void update();

        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            Call() = default;
            ~Call() = default;

            template<typename T>
            requires std::is_same_v<T, Register> || std::is_same_v<T, Immediate>
            Call &setValue(const T &value) {
                _value = value;
                update();
                return *this;
            }

    };

}
