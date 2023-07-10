/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Section
*/

#include "elf/section/Section.hpp"

namespace raven::elf {

    Section::Section() {
        _header = NULL;
    }

    Section &Section::pushData(uint8_t data) {
        _data.push_back(data);
        return *this;
    }

    Section &Section::pushData(const std::vector<uint8_t> &data) {
        _data.insert(_data.end(), data.begin(), data.end());
        return *this;
    }

    std::vector<uint8_t> &Section::getData() {
        return _data;
    }

    size_t Section::getSize() const {
        return _data.size();
    }

    void Section::dump(std::fstream &file) {
        file.write(reinterpret_cast<const char *>(_data.data()), _data.size());
    }

    void Section::bakeHeader(SectionHeader &header) {
        _header = &header;
        header.setSize(_data.size());
    }

}
