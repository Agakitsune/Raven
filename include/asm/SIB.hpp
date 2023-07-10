/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** SIB
*/

#pragma once

#include <cstdint>

namespace raven::ASM {

    class SIB {
        uint8_t _scale = 0;
        uint8_t _index = 0;
        uint8_t _base = 0;

        public:
            SIB() = default;
            ~SIB() = default;

            SIB &scale(uint8_t value);
            SIB &index(uint8_t value);
            SIB &base(uint8_t value);

            uint8_t getByte() const;

    };

}
