/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Text
*/

#include <iostream>

#include "elf/section/Text.hpp"

namespace raven::elf {

    Text &Text::addInstruction(const std::shared_ptr<ASM::Instruction> &instruction) {
        code.push_back(instruction);
        return *this;
    }

    size_t Text::getSize() const {
        size_t size = Section::getSize();
        for (const auto &ptr : code)
            size += ptr->getSize();
        return size;
    }

    void Text::dump(std::fstream &file) {
        for (auto &ptr : code)
            ptr->dump(_data);
        Section::dump(file);
    }

    void Text::bakeHeader(SectionHeader &header) {
        _header = &header;
        size_t s = _data.size();
        for (auto &ptr : code) {
            s += ptr->getSize();
        }
        header.setType(SectionHeaderType::SProgbits)
            .setFlags(SectionHeaderFlag::Alloc | SectionHeaderFlag::Execinstr)
            .setAddr(0)
            .setSize(s)
            .setLink(0)
            .setInfo(0)
            .setAddralign(16)
            .setEntsize(0);
    }

}
