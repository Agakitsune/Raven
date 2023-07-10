/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Register
*/

#include "asm/Register.hpp"

namespace raven::ASM {

    bool operator==(const Register &lhs, const Register &rhs) {
        return lhs.reg == rhs.reg && lhs.size == rhs.size && lhs.useREX == rhs.useREX && lhs.extend == rhs.extend;
    }

    bool operator!=(const Register &lhs, const Register &rhs) {
        return !(lhs == rhs);
    }

}
