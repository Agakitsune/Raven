/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Syscall
*/

#pragma once

#include "asm/Instruction.hpp"
#include "asm/Register.hpp"
#include "asm/Immediate.hpp"

#include "asm/Mov.hpp"

namespace raven::ASM {

    enum SystemCall64 : uint64_t {
        SYS_READ,
        SYS_WRITE,
        SYS_OPEN,
        SYS_CLOSE,
        SYS_MMAP = 9,
        SYS_MUNMAP = 11,
        SYS_EXIT = 60,
    };

    class Syscall : public Instruction {

        template<typename... T, size_t... I>
        static void bindArg(std::tuple<T...> &tup, std::vector<std::shared_ptr<Instruction>> &code, const std::index_sequence<I...> &_) {
            std::array<Register, 6> reg = {
                RDI,
                RSI,
                RDX,
                R10,
                R8,
                R9
            };

            Mov m;

            ( code.push_back(std::make_shared<Mov>(m.setDst(reg[I]).setSrc(std::get<I>(tup)))), ...);
        }

        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            Syscall() = default;
            ~Syscall() = default;

            template<typename... T>
            requires (sizeof...(T) < 7) && (std::conjunction_v<std::disjunction<std::is_same<T, Register>, std::is_same<T, Immediate>>...>)
            static std::vector<std::shared_ptr<Instruction>> generateSyscall(const SystemCall64 &call, const T&... params) {
                std::vector<std::shared_ptr<Instruction>> code;

                std::tuple<T...> _t = std::make_tuple(params...);

                Mov m;
                m.setDst(RAX);
                m.setSrc(Immediate(static_cast<uint64_t>(call)));
                code.push_back(std::make_shared<Mov>(m));

                bindArg(_t, code, std::make_index_sequence<sizeof...(T)>{});

                code.push_back(std::make_shared<Syscall>());

                return code;
            }

    };

}
