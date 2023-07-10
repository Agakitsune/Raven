/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Symtab
*/

#pragma once

#include <vector>

#include "elf/section/Section.hpp"
#include "elf/Symbol.hpp"

namespace raven::elf {

    class Symtab : public Section {
        std::vector<Symbol> _symbols = {
            Symbol().setName(0).setInfo(static_cast<SymbolBinding>(0)).setOther(0).setShndx(0).setSize(0).setValue(0)
        };

        public:
            Symtab() = default;
            virtual ~Symtab() override = default;

            Symtab &pushSymbol(const Symbol &symbol);
            Symbol &getSymbol(std::size_t index);
            Symbol &getSymbol(const std::string &name);

            virtual void dump(std::fstream &file) override;
            virtual void bakeHeader(SectionHeader &header) override;

    };

}
