/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Immediate
*/

#pragma once

#include <cstdint>
#include <vector>
#include <bit>
#include <cstring>
#include <limits>
#include <iostream>

namespace raven::ASM {

    // Value have been set only to dump it faster

    enum ImmediateSize {
        IMM8 = 1,
        IMM16 = 2,
        IMM32 = 4,
        IMM64 = 8
    };

    class Immediate {
        int64_t _value = 0;

        ImmediateSize _size = IMM8;

        public:
            Immediate() = default;

            template<typename T>
            requires std::is_integral_v<T>
            Immediate(T value) : _value(value) {
                if (value > std::numeric_limits<int8_t>::max() || value < std::numeric_limits<int8_t>::min())
                    _size = IMM16;
                if (value > std::numeric_limits<int16_t>::max() || value < std::numeric_limits<int16_t>::min())
                    _size = IMM32;
                if (value > std::numeric_limits<int32_t>::max() || value < std::numeric_limits<int32_t>::min())
                    _size = IMM64;
            }
            ~Immediate() = default;

            template<typename T>
            requires std::is_integral_v<T>
            Immediate &setValue(T value) {
                _value = value;
                return *this;
            }

            Immediate &setSize(ImmediateSize size);
            ImmediateSize getImmediateSize() const;

            template<typename T = int64_t>
            T getValue() const {
                return static_cast<T>(_value);
            }

            void dump(std::vector<uint8_t> &data) const;

    };

}
