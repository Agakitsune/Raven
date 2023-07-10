/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Literal
*/

#include <locale>
#include <iomanip>

#include "Literal.hpp"
#include "Utils.hpp"

std::ostream &operator<<(std::ostream &s, const raven::Literal &literal) {
    switch (literal.getVariant()) {
        case raven::LiteralType::Null:
            return s << "Null";
        case raven::LiteralType::Floating:
            return s << static_cast<double>(literal);
        case raven::LiteralType::Integral:
            return s << static_cast<long>(literal);
        case raven::LiteralType::String:
            return s << std::quoted(static_cast<std::string>(literal));
        case raven::LiteralType::Boolean:
            return s << std::boolalpha << static_cast<bool>(literal) << std::noboolalpha;
        case raven::LiteralType::Character:
            std::wstring_convert<raven::codecvtUTF8, char32_t> conv;
            return s << "'" << conv.to_bytes(static_cast<char32_t>(literal)) << "'";
    }
    return s;
}
