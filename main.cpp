/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** main
*/

#include <filesystem>

#include "HsUtils.hpp"
#include "Unserializer.hpp"
#include "Input.hpp"

#include "elf/Elf.hpp"
#include "elf/Symbol.hpp"
#include "elf/section/StrSection.hpp"
#include "elf/section/Symtab.hpp"
#include "elf/section/Text.hpp"

#include "asm/Add.hpp"
#include "asm/Inc.hpp"
#include "asm/Syscall.hpp"
#include "asm/Ret.hpp"
#include "asm/Push.hpp"
#include "asm/Pop.hpp"
#include "std/Std.hpp"

#include "Compiler.hpp"

#include <iostream>
#include <iomanip>
#include <memory>

int main(int argc, char *argv[]) {
    return raven::Compiler::compile(argc, argv);
}