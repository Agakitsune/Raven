/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** StrSection
*/

#pragma once

#include "elf/section/Section.hpp"
#include "elf/SectionHeader.hpp"
#include "elf/Symbol.hpp"

#include <iostream>

namespace raven::elf {

    class StrSection : public Section {
        public:
            StrSection() = default;
            virtual ~StrSection() override = default;

            template<typename... T>
            requires std::conjunction_v<std::is_base_of<Symbol, T>...>
            StrSection &streamSymbol(T &...symbols) {
                if (_data.size() == 0)
                    _data.push_back('\0');
                (
                    (
                        symbols.setName(_data.size()),
                        _data.insert(_data.end(), symbols.getName().begin(), symbols.getName().end()),
                        _data.push_back('\0')
                    ),
                ...);
                if (_header)
                    _header->setSize(_data.size());
                return *this;
            }

            template<typename... T>
            requires std::conjunction_v<std::is_base_of<SectionHeader, T>...>
            StrSection &streamSectionHeader(T &...sections) {
                if (_data.size() == 0)
                    _data.push_back('\0');
                (
                    (
                        sections.setName(_data.size()),
                        _data.insert(_data.end(), sections.getName().begin(), sections.getName().end()),
                        _data.push_back('\0')
                    ),
                ...);
                if (_header)
                    _header->setSize(_data.size());
                return *this;
            }

            virtual void bakeHeader(SectionHeader &header) override;

    };

}
