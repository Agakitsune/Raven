/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Elf
*/

#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "Utils.hpp"
#include "elf/ProgramHeader.hpp"
#include "elf/SectionHeader.hpp"
#include "elf/section/Section.hpp"
#include "elf/Dump.hpp"

namespace raven::elf {

    class Elf : public _private::Dump<std::fstream> {
        uint8_t _cls;
        uint8_t _data;
        uint8_t _version;
        uint8_t _osabi;
        uint8_t _abiversion;

        // Pad

        uint16_t _type;
        uint16_t _machine;
        uint32_t _version2;
        entryType::type _entry;
        entryType::type _phoff;
        entryType::type _shoff;
        uint32_t _flags;
        uint16_t _ehsize;
        uint16_t _phentsize;
        uint16_t _phnum;
        uint16_t _shentsize;
        uint16_t _shnum;
        uint16_t _shstrndx;

        size_t strtabIndex = 0;
        size_t symtabIndex = 0;

        std::vector<ProgramHeader> _programHeaders;
        std::vector<SectionHeader> _sectionHeaders;

        public:
            Elf();
            ~Elf() = default;

            Elf &setEntry(entryType::type entry);
            Elf &setShStrndx(uint16_t shstrndx);

            Elf &addProgramHeader(const ProgramHeader &programHeader);
            Elf &addSectionHeader(const SectionHeader &sectionHeader);

            Elf &setProgramHeaderOffset(entryType::type phoff);
            Elf &setSectionHeaderOffset(entryType::type phoff);

            size_t getStrtabIndex() const;

            /**
             * @brief Bake the elf file, this will set shoff based on what is in the class.
             * make sure to call this before dumping the elf file.
             *
             */
            Elf &bake();
            virtual void dump(std::fstream &file) override;

    };

}
