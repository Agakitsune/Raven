/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Move
*/

#pragma once

#include "asm/Instruction.hpp"
#include "asm/Register.hpp"

#include "asm/Mov.hpp"

namespace raven::ASM {

    enum class MoveType {
        SignExtend,
        ZeroExtend,
    };

    class Move : public Instruction {
        Register _dst;
        Register _src;

        uint8_t opcode = 0x00;
        bool D = false;

        MoveType _type = MoveType::SignExtend;

        void update();

        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            Move() = default;
            Move(const Register &dst, const Register &src, const MoveType &type = MoveType::SignExtend);
            virtual ~Move() = default;

            Move &setDst(const Register &dst);
            Move &setSrc(const Register &src);
            Move &setType(const MoveType &type);

            static std::vector<std::shared_ptr<Instruction>> move(const Register &dst, const Register &src, const MoveType &type = MoveType::SignExtend);

            static std::vector<std::shared_ptr<Instruction>> safeMove(const Register &dst, const Register &src, const MoveType &type = MoveType::SignExtend);

    };

}
