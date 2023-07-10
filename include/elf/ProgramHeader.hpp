/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Program
*/

#pragma once

#include <cstdint>

#include "Utils.hpp"
#include "elf/Dump.hpp"

namespace raven::elf {

    enum class ProgramHeaderType : uint32_t {
        PNull,
        PLoad,
        PDynamic,
        PInterp,
        PNote,
        PShlib,
        PPhdr,
        PTls,
        PLoos = 0x60000000,
        PHiOs = 0x6FFFFFFF,
        PLoProc = 0x70000000,
        PHiProc = 0x7FFFFFFF
    };

    enum class ProgramHeaderFlag : uint32_t {
        Executable = 0x1,
        Writable = 0x2,
        Readable = 0x4,
        MaskProc = 0xF0000000,
        MaskOs = 0x0FFFFFFF
    };

    namespace _private {

        struct _32ProgramHeader {
            ProgramHeaderType _type;
            uint32_t _offset;
            uint32_t _vaddr;
            uint32_t _paddr;
            uint32_t _filesz;
            uint32_t _memsz;
            uint32_t _flags;
            uint32_t _align;

            void init();

        };

        struct _64ProgramHeader {
            ProgramHeaderType _type;
            ProgramHeaderFlag _flags;
            uint64_t _offset;
            uint64_t _vaddr;
            uint64_t _paddr;
            uint64_t _filesz;
            uint64_t _memsz;
            uint64_t _align;

            void init();

        };

    }

    class ProgramHeader : protected std::conditional_t<sizeof(void *) == 8, _private::_64ProgramHeader, _private::_32ProgramHeader>, public _private::Dump<std::fstream> {
        public:
            ProgramHeader();
            ~ProgramHeader() = default;

            ProgramHeader &setType(ProgramHeaderType type);
            ProgramHeader &setFlags(ProgramHeaderFlag flag);

            ProgramHeader &setOffset(entryType::type offset);
            ProgramHeader &setVaddr(entryType::type vaddr);
            ProgramHeader &setPaddr(entryType::type paddr);
            ProgramHeader &setFilesz(entryType::type filesz);
            ProgramHeader &setMemsz(entryType::type memsz);
            ProgramHeader &setAlign(entryType::type align);

            virtual void dump(std::fstream &stream) override;

    };

}

raven::elf::ProgramHeaderFlag operator|(raven::elf::ProgramHeaderFlag a, raven::elf::ProgramHeaderFlag b);
