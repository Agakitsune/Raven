/*
** EPITECH PROJECT, 2023
** ù
** File description:
** Identifier
*/

#include "Identifier.hpp"
#include "Unserializer.hpp"

namespace raven {

    void Identifier::unserializeId(Input &input) {
        _id = unserialize<std::string>(input);
    }

    const std::string &Identifier::getId() const {
        return _id;
    }

}

std::ostream &operator<<(std::ostream &s, const raven::Identifier &id) {
    return s << id.getId();
}
