/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Literal
*/

#pragma once

#include <string>
#include <variant>
#include <memory>
#include <iostream>

#include "Input.hpp"

namespace raven {

    enum class LiteralType {
        Null,
        Floating,
        Integral,
        String,
        Boolean,
        Character,
    };

    class Literal {
        std::variant<
            std::monostate,
            double,
            long,
            std::string,
            bool,
            char32_t
        > _value;

        public:
            Literal() = default;
            ~Literal() = default;

            template<typename T>
            void setValue(T value) {
                _value = value;
            }

            template<typename T>
            operator T() const {
                return std::get<T>(_value);
            }

            constexpr LiteralType getVariant() const {
                return static_cast<LiteralType>(_value.index());
            }

    };

}

std::ostream &operator<<(std::ostream &s, const raven::Literal &literal);
