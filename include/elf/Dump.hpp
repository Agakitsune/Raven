/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Dump
*/

#pragma once

#include <fstream>
#include <vector>

namespace raven::elf::_private {

    template<typename T>
    class Dump {
        public:
            Dump() = default;
            ~Dump() = default;

            virtual void dump(T &file) = 0;

    };

}
