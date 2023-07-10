/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Shift
*/

#pragma once

#include <variant>

#include "asm/Instruction.hpp"
#include "asm/Register.hpp"
#include "asm/Immediate.hpp"

#include "asm/Move.hpp"

namespace raven::ASM {

    enum ShiftType {
        Right,
        Left
    };

    class Shift : public Instruction {
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
            Shift() = default;
            template<typename T>
            Shift(const Register &dst, const T &src, const ShiftType &type = ShiftType::Left) : _dst(dst), _src(src), _type(type) {
                update();
            }
            ~Shift() = default;

            Shift &setDst(const Register &dst);

            template<typename T>
            requires std::is_same_v<T, Register> || std::is_same_v<T, Immediate>
            Shift &setSrc(const T &src) {
                _src = src;
                update();
                return *this;
            }

            Shift &setType(const ShiftType &type);

            static std::vector<std::shared_ptr<Instruction>> sal(const Register &dst, const Register &src);
            static std::vector<std::shared_ptr<Instruction>> sal(const Register &dst, const int8_t n);
            static std::vector<std::shared_ptr<Instruction>> sar(const Register &dst, const Register &src);
            static std::vector<std::shared_ptr<Instruction>> sar(const Register &dst, const int8_t n);

    };

}
