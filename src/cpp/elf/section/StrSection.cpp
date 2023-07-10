/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** StrSection
*/

#include "elf/section/StrSection.hpp"

namespace raven::elf {

    void StrSection::bakeHeader(SectionHeader &header) {
        _header = &header;
        header.setType(SectionHeaderType::SStrtab)
            // .setFlags(SectionHeaderFlag::Strings)
            .setAddr(0)
            .setSize(_data.size())
            .setLink(0)
            .setInfo(0)
            .setAddralign(0x01)
            .setEntsize(0);
    }

}
