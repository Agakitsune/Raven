/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Immediate
*/

#include "asm/Immediate.hpp"

namespace raven::ASM {

    Immediate &Immediate::setSize(ImmediateSize size) {
        _size = size;
        return *this;
    }

    ImmediateSize Immediate::getImmediateSize() const {
        return _size;
    }

    void Immediate::dump(std::vector<uint8_t> &data) const {
        uint8_t tmp[8] = {0};
        std::memcpy(tmp, &_value, 8);
        int size = static_cast<int>(_size);
        for (int i = 0; i < size; ++i) {
            data.push_back(tmp[i]);
        }
    }

}
