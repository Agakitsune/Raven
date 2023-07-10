/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Convert
*/

#pragma once

#include "asm/Instruction.hpp"

namespace raven::ASM {

    enum class ConvertType {
        CBW,
        CWDE,
        CDQE,
    };

    class Convert : public Instruction {
        ConvertType _type = ConvertType::CBW;

        uint8_t opcode = 0x00;

        void update();

        protected:

            virtual size_t getOpcodeSize() const override;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const override;
            virtual void dumpOther(std::vector<uint8_t> &data) const override;

        public:
            Convert();
            Convert(const ConvertType &type);
            ~Convert() = default;

            Convert &setType(ConvertType type);

    };

}
