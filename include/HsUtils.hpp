/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** HsUtils
*/

#pragma once

#include <HsFFI.h>

extern "C" {
    void hs_init(int *argc, char **argv[]);
    void hs_exit();
	HsPtr ravenSerialize(HsPtr a1);
}
