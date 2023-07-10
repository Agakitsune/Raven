/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Access
*/

#pragma once

#include <variant>

#include "asm/Register.hpp"
#include "asm/Immediate.hpp"

namespace raven::ASM {

    enum class AccessFactor {
        Factor1,
        Factor2,
        Factor4,
        Factor8
    };

    class Access {
        std::variant<Register, Immediate> _index = Immediate(0);
        AccessFactor _fac = AccessFactor::Factor1;
        std::variant<std::monostate, Register, Immediate> _offset;

        public:
            Access() = default;
            Access(const Register &value, const AccessFactor &factor);
            Access(const Immediate &value, const AccessFactor &factor);
            ~Access() = default;

            Access &setValue(const Register &value);
            Access &setValue(const Immediate &value);

            Access &setFactor(const AccessFactor &factor);

            Access &setOffset(const Register &off);
            Access &setOffset(const Immediate &off);

            const std::variant<Register, Immediate> &getValue() const;
            AccessFactor getAccessFactor() const;
            const std::variant<std::monostate, Register, Immediate> &getOffset() const;

    };

}
