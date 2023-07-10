/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** SectionHeader
*/

#include <iostream>

#include "elf/SectionHeader.hpp"
#include "elf/section/Section.hpp"

namespace raven::elf {

    SectionHeader::SectionHeader() {
        _name = 0;
        _type = SectionHeaderType::SNull;
        _flags = static_cast<SectionHeaderFlag>(0);
        _addr = 0;
        _offset = 0;
        _size = 0;
        _link = 0;
        _info = 0;
        _addralign = 0;
        _entsize = 0;
        _section = NULL;
    }

    SectionHeader &SectionHeader::setName(const std::string &name) {
        _nameStr = name;
        return *this;
    }

    SectionHeader &SectionHeader::setName(uint32_t name) {
        _name = name;
        return *this;
    }

    SectionHeader &SectionHeader::setType(SectionHeaderType type) {
        _type = type;
        return *this;
    }

    SectionHeader &SectionHeader::setFlags(SectionHeaderFlag flags) {
        _flags = flags;
        return *this;
    }

    SectionHeader &SectionHeader::setAddr(entryType::type addr) {
        _addr = addr;
        return *this;
    }

    SectionHeader &SectionHeader::setOffset(entryType::type offset) {
        _offset = offset;
        return *this;
    }

    SectionHeader &SectionHeader::setSize(entryType::type size) {
        _size = size;
        return *this;
    }

    SectionHeader &SectionHeader::setLink(uint32_t link) {
        _link = link;
        return *this;
    }

    SectionHeader &SectionHeader::setInfo(uint32_t info) {
        _info = info;
        return *this;
    }

    SectionHeader &SectionHeader::setAddralign(entryType::type addralign) {
        _addralign = addralign;
        return *this;
    }

    SectionHeader &SectionHeader::setEntsize(entryType::type entsize) {
        _entsize = entsize;
        return *this;
    }

    const std::string &SectionHeader::getName() const {
        return _nameStr;
    }

    entryType::type SectionHeader::getSize() const {
        return _size;
    }

    SectionHeader &SectionHeader::bindSection(Section *section) {
        _section = section;
        _section->bakeHeader(*this);
        return *this;
    }

    size_t SectionHeader::getSectionSize() const {
        if (_section)
            return _section->getSize();
        return 0;
    }

    void SectionHeader::dumpSection(std::fstream &file) {
        if (_section) {
            std::streampos pos = file.tellg();
            if (_offset < pos) {
                std::cerr << "Section (" << std::hex << _offset << ")" << _nameStr << " is overlapping. Setting offset to 0x" << std::hex << pos << std::endl;
                _offset = pos;
            } else {
                size_t diff = _offset - pos;
                for (size_t i = 0; i < diff; ++i)
                    file.write("\0", 1);
            }
            _section->dump(file);
        }
    }

    void SectionHeader::dump(std::fstream &file) {
        file.write(reinterpret_cast<const char *>(&_name), sizeof(_name));
        file.write(reinterpret_cast<const char *>(&_type), sizeof(_type));
        file.write(reinterpret_cast<const char *>(&_flags), sizeof(_flags));
        file.write(reinterpret_cast<const char *>(&_addr), sizeof(_addr));
        file.write(reinterpret_cast<const char *>(&_offset), sizeof(_offset));
        file.write(reinterpret_cast<const char *>(&_size), sizeof(_size));
        file.write(reinterpret_cast<const char *>(&_link), sizeof(_link));
        file.write(reinterpret_cast<const char *>(&_info), sizeof(_info));
        file.write(reinterpret_cast<const char *>(&_addralign), sizeof(_addralign));
        file.write(reinterpret_cast<const char *>(&_entsize), sizeof(_entsize));
    }

}

raven::elf::SectionHeaderFlag operator|(raven::elf::SectionHeaderFlag a, raven::elf::SectionHeaderFlag b) {
    return static_cast<raven::elf::SectionHeaderFlag>(static_cast<raven::entryType::type>(a) | static_cast<raven::entryType::type>(b));
}
