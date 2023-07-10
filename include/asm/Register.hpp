/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Register
*/

#pragma once

#include "asm/REX.hpp"

namespace raven::ASM {

    enum RegisterSize {
        S8,
        S16,
        S32,
        S64,
        S128
    };

    struct Register {
        uint8_t reg;
        RegisterSize size;
        bool useREX = false;
        bool extend = false;
    };

    bool operator==(const Register &lhs, const Register &rhs);
    bool operator!=(const Register &lhs, const Register &rhs);

    const Register AL        (0, S8);
    const Register AH        (4, S8);
    const Register AX        (0, S16);
    const Register EAX       (0, S32);
    const Register RAX       (0, S64);
    const Register MM0       (0, S64);
    const Register XMM0      (0, S128);

    const Register CL      (1, S8);
    const Register CH      (5, S8);
    const Register CX      (1, S16);
    const Register ECX     (1, S32);
    const Register RCX     (1, S64);
    const Register MM1     (1, S64);
    const Register XMM1    (1, S128);
    const Register AT_ECX  (1, S32, true);
    const Register AT_RCX  (1, S64, true);

    const Register DL   (2, S8);
    const Register DH   (6, S8);
    const Register DX   (2, S16);
    const Register EDX  (2, S32);
    const Register RDX  (2, S64);
    const Register MM2  (2, S64);
    const Register XMM2 (2, S128);

    const Register BL   (3, S8);
    const Register BH   (7, S8);
    const Register BX   (3, S16);
    const Register EBX  (3, S32);
    const Register RBX  (3, S64);
    const Register MM3  (3, S64);
    const Register XMM3 (3, S128);

    const Register SPL  (4, S8, true);
    const Register SP   (4, S16);
    const Register ESP  (4, S32);
    const Register RSP  (4, S64);
    const Register MM4  (4, S64);
    const Register XMM4 (4, S128);

    const Register BPL  (5, S8, true);
    const Register BP   (5, S16);
    const Register EBP  (5, S32);
    const Register RBP  (5, S64);
    const Register MM5  (5, S64);
    const Register XMM5 (5, S128);

    const Register SIL  (6, S8, true);
    const Register SI   (6, S16);
    const Register ESI  (6, S32);
    const Register RSI  (6, S64);
    const Register MM6  (6, S64);
    const Register XMM6 (6, S128);

    const Register DIL  (7, S8, true);
    const Register DI   (7, S16);
    const Register EDI  (7, S32);
    const Register RDI  (7, S64);
    const Register MM7  (7, S64);
    const Register XMM7 (7, S128);

    const Register R8B  (0, S8, true, true);
    const Register R8W  (0, S16, true, true);
    const Register R8D  (0, S32, true, true);
    const Register R8   (0, S64, true, true);

    const Register R9B  (1, S8, true, true);
    const Register R9W  (1, S16, true, true);
    const Register R9D  (1, S32, true, true);
    const Register R9   (1, S64, true, true);

    const Register R10B (2, S8, true, true);
    const Register R10W (2, S16, true, true);
    const Register R10D (2, S32, true, true);
    const Register R10  (2, S64, true, true);

    const Register R11B (3, S8, true, true);
    const Register R11W (3, S16, true, true);
    const Register R11D (3, S32, true, true);
    const Register R11  (3, S64, true, true);

    const Register R12B (4, S8, true, true);
    const Register R12W (4, S16, true, true);
    const Register R12D (4, S32, true, true);
    const Register R12  (4, S64, true, true);

    const Register R13B (5, S8, true, true);
    const Register R13W (5, S16, true, true);
    const Register R13D (5, S32, true, true);
    const Register R13  (5, S64, true, true);

    const Register R14B (6, S8, true, true);
    const Register R14W (6, S16, true, true);
    const Register R14D (6, S32, true, true);
    const Register R14  (6, S64, true, true);

    const Register R15B (7, S8, true, true);
    const Register R15W (7, S16, true, true);
    const Register R15D (7, S32, true, true);
    const Register R15  (7, S64, true, true);

}
