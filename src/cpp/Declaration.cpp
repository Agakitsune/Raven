/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Declaration
*/

#include "Declaration.hpp"

namespace raven {

    DeclarationType Declaration::getDeclarationType() const {
        return _type;
    }

    const DeclarationBehavior &Declaration::getDeclaration() const {
        return *_behavior;
    }

}

std::ostream &operator<<(std::ostream &s, const raven::Declaration &decl) {
    return s << decl.getDeclaration();
}
