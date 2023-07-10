/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Block
*/

#pragma once

#include <memory>
#include <vector>
#include <iostream>

#include "Input.hpp"

namespace raven {

    class Statement;

    class Block {
        std::vector<std::shared_ptr<Statement>> _statements;

        public:
            Block() = default;
            ~Block() = default;

            void unserializeBlk(Input &input);

            const std::vector<std::shared_ptr<Statement>> &getStatements() const;

    };

}

std::ostream &operator<<(std::ostream &s, const raven::Block &blk);
