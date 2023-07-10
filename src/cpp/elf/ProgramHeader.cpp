/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Program
*/

#include "elf/ProgramHeader.hpp"

namespace raven::elf {

    namespace _private {

        void _32ProgramHeader::init() {
            _type = ProgramHeaderType::PNull;
            _offset = 0;
            _vaddr = 0;
            _paddr = 0;
            _filesz = 0;
            _memsz = 0;
            _flags = 0;
            _align = 0;
        }

        void _64ProgramHeader::init() {
            _type = ProgramHeaderType::PNull;
            _flags = static_cast<ProgramHeaderFlag>(0);
            _offset = 0;
            _vaddr = 0;
            _paddr = 0;
            _filesz = 0;
            _memsz = 0;
            _align = 0;
        }

    }

    ProgramHeader::ProgramHeader() {
        init();
    }

    ProgramHeader &ProgramHeader::setType(ProgramHeaderType type) {
        _type = type;
        return *this;
    }

    ProgramHeader &ProgramHeader::setFlags(ProgramHeaderFlag flags) {
        if constexpr (std::is_base_of_v<_private::_64ProgramHeader, ProgramHeader>)
            _flags = flags;
        return *this;
    }

    ProgramHeader &ProgramHeader::setOffset(entryType::type offset) {
        _offset = offset;
        return *this;
    }

    ProgramHeader &ProgramHeader::setVaddr(entryType::type vaddr) {
        _vaddr = vaddr;
        return *this;
    }

    ProgramHeader &ProgramHeader::setPaddr(entryType::type paddr) {
        _paddr = paddr;
        return *this;
    }

    ProgramHeader &ProgramHeader::setFilesz(entryType::type filesz) {
        _filesz = filesz;
        return *this;
    }

    ProgramHeader &ProgramHeader::setMemsz(entryType::type memsz) {
        _memsz = memsz;
        return *this;
    }

    ProgramHeader &ProgramHeader::setAlign(entryType::type align) {
        _align = align;
        return *this;
    }

    void ProgramHeader::dump(std::fstream &file) {
        file.write(reinterpret_cast<const char *>(&_type), sizeof(this->_type));
        if constexpr (std::is_base_of_v<_private::_64ProgramHeader, ProgramHeader>)
            file.write(reinterpret_cast<const char *>(&_flags), sizeof(this->_flags));
        file.write(reinterpret_cast<const char *>(&_offset), sizeof(this->_offset));
        file.write(reinterpret_cast<const char *>(&_vaddr), sizeof(this->_vaddr));
        file.write(reinterpret_cast<const char *>(&_paddr), sizeof(this->_paddr));
        file.write(reinterpret_cast<const char *>(&_filesz), sizeof(this->_filesz));
        file.write(reinterpret_cast<const char *>(&_memsz), sizeof(this->_memsz));
        file.write(reinterpret_cast<const char *>(&_align), sizeof(this->_align));
    }

}

raven::elf::ProgramHeaderFlag operator|(raven::elf::ProgramHeaderFlag a, raven::elf::ProgramHeaderFlag b) {
    return static_cast<raven::elf::ProgramHeaderFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
