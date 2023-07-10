/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Std
*/

#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <functional>

#include "FunctionPrototype.hpp"

#include "asm/Label.hpp"
#include "elf/Symbol.hpp"
#include "behavior/DeclarationBehavior.hpp"

#include "exception/CompilerException.hpp"

namespace raven::stdlib {

    // Query

    std::vector<elf::Symbol> getStandardLibrarySymbols();
    std::vector<uint8_t> getStandardLibrary();
    std::vector<FunctionPrototype> getStandardLibraryDeclarations();

    // Private function

    uint8_t *__allocateString();
    uint8_t *__deallocateString();
    uint8_t *__copyString();
    uint8_t *__displayString();

    // Public function

    uint8_t *read();
    uint8_t *write();
    uint8_t *open();
    uint8_t *close();
    uint8_t *exit();

    uint8_t *length();

    uint8_t *print();
    uint8_t *println();

}
