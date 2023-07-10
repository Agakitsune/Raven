/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** LiteralBehaviour
*/

#pragma once

#include "Input.hpp"
#include "Literal.hpp"
#include "exception/SerializationException.hpp"
#include "Unserializer.hpp"

namespace raven {

    class NumberLiteralBehavior {
        public:
            NumberLiteralBehavior() = default;
            ~NumberLiteralBehavior() = default;

            static void unserializeliteral(Literal &literal, Input &input);
    };

    class StringLiteralBehavior {
        public:
            StringLiteralBehavior() = default;
            ~StringLiteralBehavior() = default;

            static void unserializeliteral(Literal &literal, Input &input);
    };

    class BooleanLiteralBehavior {
        public:
            BooleanLiteralBehavior() = default;
            ~BooleanLiteralBehavior() = default;

            static void unserializeliteral(Literal &literal, Input &input);
    };

    class CharacterLiteralBehavior {
        public:
            CharacterLiteralBehavior() = default;
            ~CharacterLiteralBehavior() = default;

            static void unserializeliteral(Literal &literal, Input &input);
    };

    class NullLiteralBehavior {
        public:
            NullLiteralBehavior() = default;
            ~NullLiteralBehavior() = default;

            static void unserializeliteral(Literal &literal, Input &input);
    };

}
