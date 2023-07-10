/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** REX
*/

#pragma once

#include <cstdint>

namespace raven::ASM {

    class REX {
        bool _w = false;
        bool _r = false;
        bool _x = false;
        bool _b = false;

        public:
            REX() = default;
            ~REX() = default;

            REX &W(bool value);
            REX &R(bool value);
            REX &X(bool value);
            REX &B(bool value);

            uint8_t getByte() const;

    };

}
