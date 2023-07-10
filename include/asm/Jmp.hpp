/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Jmp
*/

#pragma once

#include <variant>

#include "asm/Instruction.hpp"
#include "asm/Register.hpp"
#include "asm/Immediate.hpp"

namespace raven::ASM {

    enum Condition {
        None,
        Above,
        AboveEqual,
        Below,
        BelowEqual,
        Carry,
        ZeroECX,
        ZeroRCX,
        Equal,
        Greater,
        GreaterEqual,
        Lower,
        LowerEqual,
        NotAbove,
        NotAboveEqual,
        NotBelow,
        NotBelowEqual,
        NotCarry,
        NotEqual,
        NotGreater,
        NotGreaterEqual,
        NotLower,
        NotLowerEqual,
        NotOverflow,
        NotParity,
        NotSign,
        NotZero,
        Overflow,
        Parity,
        ParityEven,
        ParityOdd,
    };

    class Jmp : public Instruction {
        std::variant<Register, Immediate> _value = Immediate(0);
        Condition _cond = None;

        uint8_t opcode = 0x0;
        uint8_t opcode2 = 0x0;

        void update();

        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            Jmp() = default;
            Jmp(const Condition &condition);
            template<typename T>
            Jmp(const T &value, const Condition &condition = None) : _value(value), _cond(condition) {
                update();
            }
            ~Jmp() = default;

            template<typename T>
            requires std::is_same_v<T, Register> || std::is_same_v<T, Immediate>
            Jmp &setValue(const T &value) {
                _value = value;
                update();
                return *this;
            }

            Jmp &setCondition(const Condition &condition);

    };

}
