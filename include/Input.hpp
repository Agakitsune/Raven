/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Input
*/

#pragma once

#include <cstdint>
#include <cstddef>

namespace raven {

    class Input {
        uint8_t *data;
        size_t offset = 0;

        public:
            Input(uint8_t *data);
            ~Input() = default;

            void consume(size_t n);
            uint8_t *getData() const;

    };

}
