/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Symtab
*/

#include "elf/section/Symtab.hpp"

namespace raven::elf {

    Symtab &Symtab::pushSymbol(const Symbol &symbol) {
        _symbols.push_back(symbol);
        if (_header)
            _header->setSize(_symbols.size() * Symbol::size);
        return *this;
    }

    Symbol &Symtab::getSymbol(std::size_t index) {
        return _symbols[index];
    }

    Symbol &Symtab::getSymbol(const std::string &name) {
        for (auto &symbol : _symbols)
            if (symbol.getName() == name)
                return symbol;
        throw std::runtime_error("Symbol not found");
    }

    void Symtab::dump(std::fstream &file) {
        for (auto &symbol : _symbols)
            symbol.dump(file);
    }

    void Symtab::bakeHeader(SectionHeader &header) {
        _header = &header;
        header.setType(SectionHeaderType::SSymtab)
            // .setFlags(SectionHeaderFlag::Alloc)
            .setAddr(0)
            .setSize(_symbols.size() * Symbol::size)
            .setLink(0)
            .setInfo(0)
            .setAddralign(8)
            .setEntsize(Symbol::size);
    }

}
