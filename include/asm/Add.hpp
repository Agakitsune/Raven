/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Add
*/

#pragma once

#include <variant>

#include "asm/Instruction.hpp"
#include "asm/Register.hpp"
#include "asm/Immediate.hpp"

namespace raven::ASM {

    class Add : public Instruction {
        Register _dst;
        std::variant<Register, Immediate> _src = Immediate(0);

        uint8_t opcode = 0xff;

        void update();

        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            Add() = default;
            template<typename T>
            Add(const Register &dst, const T &src) : _dst(dst), _src(src) {
                update();
            }
            ~Add() = default;

            Add &setDst(const Register &dst);

            template<typename T>
            requires std::is_same_v<T, Register> || std::is_same_v<T, Immediate>
            Add &setSrc(const T &src) {
                _src = src;
                update();
                return *this;
            }

            static std::vector<std::shared_ptr<Instruction>> add(const Register &dst, const Register &src);
            static std::vector<std::shared_ptr<Instruction>> add(const Register &dst, const uint64_t n);

    };

}