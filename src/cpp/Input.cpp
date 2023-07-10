/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Input
*/

#include "Input.hpp"

namespace raven {

    Input::Input(uint8_t *data): data(data) {}

    void Input::consume(size_t n) {
        offset += n;
    }

    uint8_t *Input::getData() const {
        return data + offset;
    }

}
