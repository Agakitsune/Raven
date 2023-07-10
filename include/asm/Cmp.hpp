/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Cmp
*/

#pragma once

#include <variant>

#include "asm/Instruction.hpp"
#include "asm/Register.hpp"
#include "asm/Immediate.hpp"

namespace raven::ASM {

    class Cmp : public Instruction {
        Register _dst;
        std::variant<Register, Immediate> _src = Immediate(0);

        uint8_t opcode = 0xff;

        void update();

        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            Cmp() = default;
            template<typename T>
            Cmp(const Register &dst, const T &src) : _dst(dst), _src(src) {
                update();
            }
            ~Cmp() = default;

            Cmp &setDst(const Register &dst);

            template<typename T>
            requires std::is_same_v<T, Register> || std::is_same_v<T, Immediate>
            Cmp &setSrc(const T &src) {
                _src = src;
                update();
                return *this;
            }

    };

}
