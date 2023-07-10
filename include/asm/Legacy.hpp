/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Legacy
*/

#pragma once

#include <cstdint>

namespace raven::ASM {

    /**
     * @brief Didn't really understand why or how to use it
     * but Legacy Group 3 Operand-Size override is present when using 16-bit operator
     *
     */
    struct Legacy {
        const uint8_t grp;
        const uint8_t byte;
    };

    // Group 1

    const Legacy LOCK           (0, 0xf0);
    const Legacy REPNZ          (0, 0xf2);
    const Legacy REP            (0, 0xf3);

    // Group 2

    const Legacy CSOverride     (1, 0x2e);
    const Legacy SSOverride     (1, 0x36);
    const Legacy DSOverride     (1, 0x3e);
    const Legacy ESOverride     (1, 0x26);
    const Legacy FSOverride     (1, 0x64);
    const Legacy GSOverride     (1, 0x65);

    /**
     * @brief Branch not taken
     *
     */
    const Legacy BRCHNT         (1, 0x2e);

    /**
     * @brief Branch taken
     *
     */
    const Legacy BRCHT          (1, 0x3e);

    // Group 3

    const Legacy OPOverride     (2, 0x66);

    // Group 4

    const Legacy ADDROverride   (3, 0x67);

}
