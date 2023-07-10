/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Mov
*/

#pragma once

#include <variant>

#include "asm/Instruction.hpp"
#include "asm/Register.hpp"
#include "asm/Immediate.hpp"
#include "asm/Access.hpp"

namespace raven::ASM {

    class Mov : public Instruction {
        std::variant<Register, Access> _dst;
        std::variant<Register, Immediate, Access> _src = Immediate(0);

        uint8_t opcode = 0x00;

        void update();

        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            Mov() = default;
            template<typename T, typename T2>
            Mov(const T &dst, const T2 &src) : _dst(dst), _src(src) {
                update();
            }
            ~Mov() = default;

            template<typename T>
            requires std::is_same_v<T, Register> || std::is_same_v<T, Access>
            Mov &setDst(const T &dst) {
                _dst = dst;
                update();
                return *this;
            }

            template<typename T>
            requires std::is_same_v<T, Register> || std::is_same_v<T, Immediate> || std::is_same_v<T, Access>
            Mov &setSrc(const T &src) {
                _src = src;
                update();
                return *this;
            }

    };

}
