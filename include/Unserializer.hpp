/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Unserializer
*/

#pragma once

#include <string>
#include <concepts>

#include "Input.hpp"
#include "Literal.hpp"
#include "Identifier.hpp"
#include "Expression.hpp"
#include "Statement.hpp"
#include "Block.hpp"
#include "Declaration.hpp"
#include "behavior/LiteralBehavior.hpp"

namespace raven {

    template<typename T>
    T unserialize(Input &input);

    template<typename T>
    requires std::is_arithmetic_v<T>
    T unserialize(Input &input) {
        T value = *reinterpret_cast<T*>(input.getData());
        input.consume(sizeof(T));
        return value;
    }

    template<>
    long unserialize(Input &input);

    template<>
    double unserialize(Input &input);

    template<>
    std::string unserialize(Input &input);

    template<>
    Literal unserialize(Input &input);

    template<>
    Identifier unserialize(Input &input);

    template<>
    Operation unserialize(Input &input);

    template<>
    Call unserialize(Input &input);

    template<>
    Fold unserialize(Input &input);

    template<>
    Index unserialize(Input &input);

    template<>
    Array unserialize(Input &input);

    template<>
    Expression unserialize(Input &input);

    template<>
    Block unserialize(Input &input);

    template<>
    Declaration unserialize(Input &input);

    template<>
    If unserialize(Input &input);

    template<>
    While unserialize(Input &input);

    template<>
    Return unserialize(Input &input);

    template<>
    Statement unserialize(Input &input);

}
