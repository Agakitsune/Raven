import sys
import os

methods = []

for file in sys.argv[1::]:
    if (os.path.exists(file)):
        with open(file, 'r') as header:
            lines = header.readlines()
            for line in lines:
                if line.startswith("extern") and not line.count("\"C\""):
                    methods.append(line[7:])

with open("./include/HsUtils.hpp", 'w') as utils:
    utils.write(
        """/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** HsUtils
*/

#pragma once

#include <HsFFI.h>

extern "C" {
    void hs_init(int *argc, char **argv[]);
    void hs_exit();\n"""
    )

    for m in methods:
        utils.write("\t" + m)
    utils.write("}\n")
