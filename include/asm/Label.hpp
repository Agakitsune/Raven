/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Label
*/

#pragma once

#include <cstdint>

#include "asm/Immediate.hpp"

namespace raven::ASM {

    class Label {
        int64_t _offset = 0;

        public:
            Label() = default;
            ~Label() = default;

            Label &setOffset(const int64_t &offset);
            int64_t getOffset() const;

            Immediate relative(const int64_t pos, const ImmediateSize &size = IMM32);

    };

}
