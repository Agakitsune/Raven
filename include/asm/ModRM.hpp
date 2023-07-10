/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** ModRM
*/

#pragma once

#include <cstdint>
#include <memory>

#include "asm/Register.hpp"

namespace raven::ASM {

    enum class Mod : uint8_t {
        Mod00,
        Mod01,
        Mod02,
        Mod03,
    };

    class ModRM {
        Mod _mod = Mod::Mod00;
        Register _reg = EAX;
        Register _rm = EAX;

        public:
            ModRM() = default;
            ~ModRM() = default;

            ModRM &setMod(const Mod &mod);
            ModRM &setReg(const Register &reg);
            ModRM &setRm(const Register &rm);

            uint8_t getByte() const;

    };

}
