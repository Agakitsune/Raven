/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Elf
*/

#include <bit>
#include <cstring>
#include <iostream>

#include "elf/Elf.hpp"

namespace raven::elf {

    Elf::Elf() {
        if constexpr (std::is_same_v<entryType::type, uint32_t>) {
            _cls = 1;
            _phoff = 0x34;
            _ehsize = 0x34;
            _phentsize = sizeof(_private::_32ProgramHeader);
        } else {
            _cls = 2;
            _phoff = 0x40;
            _ehsize = 0x40;
            _phentsize = sizeof(_private::_64ProgramHeader);
        }
        _data = std::endian::native == std::endian::little ? 1 : 2;
        _version = 1;
        _osabi = 0;
        _abiversion = 0;
        _type = 2;
        _machine = 0x3e;
        _version2 = 1;
        _shoff = 0;
        _flags = 0; // TODO: Add flags
        _shentsize = SectionHeader::size;
        _phnum = 0;
        _shnum = 1;
        _entry = 0;

        _sectionHeaders.push_back(
            SectionHeader().setName(0)
                .setType(SectionHeaderType::SNull)
                .setFlags(static_cast<SectionHeaderFlag>(0))
                .setAddr(0)
                .setOffset(0)
                .setSize(0)
                .setLink(0)
                .setInfo(0)
                .setAddralign(0)
                .setEntsize(0)
        );
    }

    Elf &Elf::setEntry(entryType::type entry) {
        _entry = entry;
        return *this;
    }

    Elf &Elf::setShStrndx(uint16_t shstrndx) {
        _shstrndx = shstrndx;
        return *this;
    }

    Elf &Elf::addProgramHeader(const ProgramHeader &programHeader) {
        _programHeaders.push_back(programHeader);
        ++_phnum;
        return *this;
    }

    Elf &Elf::addSectionHeader(const SectionHeader &sectionHeader) {
        _sectionHeaders.push_back(sectionHeader);
        if (sectionHeader.getName() == ".shstrtab") {
            _shstrndx = _shnum;
        } else if (sectionHeader.getName() == ".strtab") {
            strtabIndex = _shnum;
            if (symtabIndex != 0)
                _sectionHeaders.at(symtabIndex).setLink(strtabIndex);
        } else if (sectionHeader.getName() == ".symtab") {
            symtabIndex = _shnum;
            if (strtabIndex != 0)
                _sectionHeaders.back().setLink(strtabIndex);
        }
        ++_shnum;
        return *this;
    }

    Elf &Elf::setProgramHeaderOffset(entryType::type phoff) {
        _phoff = phoff;
        return *this;
    }

    Elf &Elf::setSectionHeaderOffset(entryType::type shoff) {
        _shoff = shoff;
        return *this;
    }

    size_t Elf::getStrtabIndex() const {
        return strtabIndex;
    }

    Elf &Elf::bake() {
        if (_sectionHeaders.empty())
            return *this;
        _shoff = _ehsize + _phnum * _phentsize;
        for (const auto &sh : _sectionHeaders) {
                                    _shoff += sh.getSize();
        }
                return *this;
    }

    void Elf::dump(std::fstream &file) {
        const char magic[4] = {0x7f, 'E', 'L', 'F'};
        const char pad[7] = {0};
        file.write(magic, 4);
        file.write(reinterpret_cast<const char *>(&_cls), sizeof(_cls));
        file.write(reinterpret_cast<const char *>(&_data), sizeof(_data));
        file.write(reinterpret_cast<const char *>(&_version), sizeof(_version));
        file.write(reinterpret_cast<const char *>(&_osabi), sizeof(_osabi));
        file.write(reinterpret_cast<const char *>(&_abiversion), sizeof(_abiversion));
        file.write(reinterpret_cast<const char *>(pad), sizeof(pad));
        file.write(reinterpret_cast<const char *>(&_type), sizeof(_type));
        file.write(reinterpret_cast<const char *>(&_machine), sizeof(_machine));
        file.write(reinterpret_cast<const char *>(&_version2), sizeof(_version2));
        file.write(reinterpret_cast<const char *>(&_entry), sizeof(_entry));
        file.write(reinterpret_cast<const char *>(&_phoff), sizeof(_phoff));
        file.write(reinterpret_cast<const char *>(&_shoff), sizeof(_shoff));
        file.write(reinterpret_cast<const char *>(&_flags), sizeof(_flags));
        file.write(reinterpret_cast<const char *>(&_ehsize), sizeof(_ehsize));
        file.write(reinterpret_cast<const char *>(&_phentsize), sizeof(_phentsize));
        file.write(reinterpret_cast<const char *>(&_phnum), sizeof(_phnum));
        file.write(reinterpret_cast<const char *>(&_shentsize), sizeof(_shentsize));
        file.write(reinterpret_cast<const char *>(&_shnum), sizeof(_shnum));
        file.write(reinterpret_cast<const char *>(&_shstrndx), sizeof(_shstrndx));
        std::streampos pos = file.tellp();
        if (_phoff >= pos) {
            size_t diff = _phoff - pos;
            for (size_t i = 0; i < diff; ++i)
                file.write("\0", 1);
        }
        for (auto &programHeader : _programHeaders)
            programHeader.dump(file);
        for (auto &sectionHeader : _sectionHeaders) {
            sectionHeader.dumpSection(file);
        }
        pos = file.tellp();
        if (_shoff >= pos) {
            size_t diff = _shoff - pos;
            for (size_t i = 0; i < diff; ++i)
                file.write("\0", 1);
        }
        for (auto &sectionHeader : _sectionHeaders) {
            sectionHeader.dump(file);
        }
    }

}
