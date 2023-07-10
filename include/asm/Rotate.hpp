/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Rotate
*/

#pragma once

#include <variant>

#include "asm/Instruction.hpp"
#include "asm/Register.hpp"
#include "asm/Immediate.hpp"
#include "asm/Shift.hpp"
#include "asm/Mov.hpp"

namespace raven::ASM {

    class Rotate : public Instruction {
        Register _dst;
        std::variant<Register, Immediate> _src = Immediate(0);

        uint8_t opcode = 0xff;

        ShiftType _type = ShiftType::Left;

        void update();

        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            Rotate() = default;
            template<typename T>
            Rotate(const Register &dst, const T &src, const ShiftType &type = ShiftType::Left) : _dst(dst), _src(src), _type(type) {
                update();
            }
            ~Rotate() = default;

            Rotate &setDst(const Register &dst);

            template<typename T>
            requires std::is_same_v<T, Register> || std::is_same_v<T, Immediate>
            Rotate &setSrc(const T &src) {
                _src = src;
                update();
                return *this;
            }

            Rotate &setType(const ShiftType &type);

            static std::vector<std::shared_ptr<Instruction>> rol(const Register &dst, const Register &src);
            static std::vector<std::shared_ptr<Instruction>> rol(const Register &dst, const int8_t n);
            static std::vector<std::shared_ptr<Instruction>> ror(const Register &dst, const Register &src);
            static std::vector<std::shared_ptr<Instruction>> ror(const Register &dst, const int8_t n);

    };

}
