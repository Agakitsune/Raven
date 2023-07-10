/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Text
*/

#pragma once

#include <memory>

#include "elf/section/Section.hpp"

#include "asm/Instruction.hpp"

namespace raven::elf {

    class Text : public Section {
        std::vector<std::shared_ptr<ASM::Instruction>> code;

        public:
            Text() = default;
            virtual ~Text() override = default;

            Text &addInstruction(const std::shared_ptr<ASM::Instruction> &instruction);

            virtual size_t getSize() const override;

            virtual void dump(std::fstream &file) override;
            virtual void bakeHeader(SectionHeader &header) override;

    };

}
