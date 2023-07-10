/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Section
*/

#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>

#include "elf/Dump.hpp"
#include "elf/SectionHeader.hpp"

namespace raven::elf {

    class Section : public _private::Dump<std::fstream> {
        protected:
            std::vector<uint8_t> _data;

            SectionHeader *_header;

        public:
            Section();
            virtual ~Section() = default;

            Section &pushData(uint8_t data);
            Section &pushData(const std::vector<uint8_t> &data);

            std::vector<uint8_t> &getData();

            virtual size_t getSize() const;

            virtual void dump(std::fstream &file) override;

            virtual void bakeHeader(SectionHeader &header);

    };

}
