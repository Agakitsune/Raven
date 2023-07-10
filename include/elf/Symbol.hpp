/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Symbol
*/

#pragma once

#include <string>
#include <bit>

#include "Utils.hpp"
#include "elf/Dump.hpp"

#include <elf.h>

namespace raven::elf {

    enum SymbolBinding : uint8_t {
        BLocal = 0,
        BGlobal = 1,
        BWeak = 2,
        BLoos = 10,
        BHios = 12,
        BLoProc = 13,
        BHiProc = 15
    };

    enum SymbolType : uint8_t {
        TNoType = 0,
        TObject = 1,
        TFunc = 2,
        TSection = 3,
        TFile = 4,
        TCommon = 5,
        TTLS = 6,
        TLoos = 10,
        THios = 12,
        TLoProc = 13,
        THiProc = 15
    };

    enum SymbolVisibility {
        Default = 0,
        Internal = 1,
        Hidden = 2,
        Protected = 3
    };

    class Symbol : public _private::Dump<std::fstream>, public _private::Dump<std::vector<uint8_t>> {
        std::string _nameStr;
        uint32_t _name;
        entryType::type _value;
        entryType::type _size;
        uint8_t _info;
        uint8_t _other;
        uint16_t _shndx = 0;

        public:
            Symbol() = default;
            ~Symbol() = default;

            static const size_t size = sizeof(_name) + sizeof(_value) + sizeof(_size) + sizeof(_info) + sizeof(_other) + sizeof(_shndx);

            Symbol &setName(const std::string &name);
            Symbol &setName(entryType::type name);
            Symbol &setValue(entryType::type value);
            Symbol &setSize(entryType::type size);
            Symbol &setInfo(SymbolBinding binding, SymbolType type = TNoType);
            Symbol &setOther(uint8_t other);
            Symbol &setShndx(uint16_t shndx);

            const std::string &getName() const;
            entryType::type getValue() const;
            SymbolType getType() const;
            uint16_t getShndx() const;

            virtual void dump(std::fstream &file) override;
            virtual void dump(std::vector<uint8_t> &stream) override;

    };

}
