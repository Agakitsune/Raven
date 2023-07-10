/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** SectionHeader
*/

#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include "Utils.hpp"
#include "elf/Dump.hpp"

namespace raven::elf {

    class Section;

    enum class SectionHeaderType : uint32_t {
        SNull,
        SProgbits,
        SSymtab,
        SStrtab,
        SRela,
        SHash,
        SDynamic,
        SNote,
        SNoBits,
        SRel,
        SShlib,
        SDynsym,
        SInitArray = 0x0e,
        SFiniArray,
        SPreinitArray,
        SGroup,
        SSymtabShndx,
        SNum,
        SLoos = 0x60000000,
        SHios = 0x6fffffff,
        SLoProc = 0x70000000,
        SHiproc = 0x7fffffff,
        SLoUser = 0x80000000,
        SHiuser = 0xffffffff
    };

    enum class SectionHeaderFlag : entryType::type {
        Write = 0x1,
        Alloc = 0x2,
        Execinstr = 0x4,
        Merge = 0x10,
        Strings = 0x20,
        InfoLink = 0x40,
        LinkOrder = 0x80,
        OsNonconforming = 0x100,
        Group = 0x200,
        Tls = 0x400,
        MaskOs = 0x0ff00000,
        MaskProc = 0xf0000000,
        Ordered = 0x4000000,
        Exclude = 0x8000000,
        ExcludeOs = 0x0ff00000,
        ExcludeProc = 0xf0000000,
        MaskOsExclude = 0x0ff00000,
        MaskProcExclude = 0xf0000000
    };

    class SectionHeader : public _private::Dump<std::fstream> {
        std::string _nameStr;
        uint32_t _name;
        SectionHeaderType _type;
        SectionHeaderFlag _flags;
        entryType::type _addr;
        entryType::type _offset;
        entryType::type _size;
        uint32_t _link;
        uint32_t _info;
        entryType::type _addralign;
        entryType::type _entsize;

        Section *_section;

        public:
            SectionHeader();
            ~SectionHeader() = default;

            static const size_t size = sizeof(_name) + sizeof(_type) + sizeof(_flags) + sizeof(_addr) + sizeof(_offset) + sizeof(_size) + sizeof(_link) + sizeof(_info) + sizeof(_addralign) + sizeof(_entsize);

            SectionHeader &setName(const std::string &name);
            SectionHeader &setName(uint32_t name);
            SectionHeader &setType(SectionHeaderType type);
            SectionHeader &setFlags(SectionHeaderFlag flags);
            SectionHeader &setAddr(entryType::type addr);
            SectionHeader &setOffset(entryType::type offset);
            SectionHeader &setSize(entryType::type size);
            SectionHeader &setLink(uint32_t link);
            SectionHeader &setInfo(uint32_t info);
            SectionHeader &setAddralign(entryType::type addralign);
            SectionHeader &setEntsize(entryType::type entsize);

            const std::string &getName() const;
            entryType::type getSize() const;

            SectionHeader &bindSection(Section *section);
            size_t getSectionSize() const;

            void dumpSection(std::fstream &file);
            virtual void dump(std::fstream &file) override;

    };

}

raven::elf::SectionHeaderFlag operator|(raven::elf::SectionHeaderFlag a, raven::elf::SectionHeaderFlag b);
