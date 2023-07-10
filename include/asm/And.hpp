/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** And
*/

#pragma once

#include <variant>

#include "asm/Instruction.hpp"
#include "asm/Register.hpp"
#include "asm/Immediate.hpp"

namespace raven::ASM {

    class And : public Instruction {
        Register _dst;
        std::variant<Register, Immediate> _src = Immediate(0);

        uint8_t opcode = 0xff;

        void update();

        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            And() = default;
            template<typename T>
            And(const Register &dst, const T &src) : _dst(dst), _src(src) {
                update();
            }
            ~And() = default;

            And &setDst(const Register &dst);

            template<typename T>
            requires std::is_same_v<T, Register> || std::is_same_v<T, Immediate>
            And &setSrc(const T &src) {
                _src = src;
                update();
                return *this;
            }

            static std::vector<std::shared_ptr<Instruction>> and_(const Register &dst, const Register &src);
            static std::vector<std::shared_ptr<Instruction>> and_(const Register &dst, const int64_t &src);

    };

}
