/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** LiteralBehavior
*/

#include <cstring>
#include <iostream>

#include "behavior/LiteralBehavior.hpp"
#include "Utils.hpp"

namespace raven {

    void NumberLiteralBehavior::unserializeliteral(Literal &literal, Input &input) {
        uint8_t type = unserialize<uint8_t>(input);
        long ivalue = 0.0;
        double fvalue = 0.0;
        switch (type) {
            case 0:
                ivalue = unserialize<long>(input);
                literal.setValue(ivalue);
                break;
            case 1:
                fvalue = unserialize<double>(input);
                literal.setValue(fvalue);
                break;
            default:
                throw exception::SerializationException("Invalid number type");
        }
    }

    void StringLiteralBehavior::unserializeliteral(Literal &literal, Input &input) {
        std::string value = unserialize<std::string>(input);
        literal.setValue(value);
    }

    void BooleanLiteralBehavior::unserializeliteral(Literal &literal, Input &input) {
        bool value = unserialize<bool>(input);
        literal.setValue(value);
    }

    void CharacterLiteralBehavior::unserializeliteral(Literal &literal, Input &input) {
        uint8_t size = unserialize<uint8_t>(input);
        char hold[size + 1];
        std::memset(hold, 0, size + 1);
        for (uint8_t i = 0; i < size; i++)
            hold[i] = unserialize<uint8_t>(input);
        std::wstring_convert<codecvtUTF8, char32_t> conv;
        std::u32string str = conv.from_bytes(hold);
        char32_t value = str.at(0);
        literal.setValue(value);
    }

    void NullLiteralBehavior::unserializeliteral(Literal &literal, Input &input) {
        (void)input;
        literal.setValue(std::monostate());
    }

}
