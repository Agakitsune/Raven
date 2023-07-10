/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Statement
*/

#include "Statement.hpp"

std::ostream &displayDeclaration(std::ostream &s, const raven::Statement &statement) {
    raven::Declaration decl = static_cast<raven::Declaration>(statement);
    if (decl.getDeclarationType() == raven::DeclarationType::Variable)
        return s << decl << ";";
    else
        return s << decl;
}

std::ostream &operator<<(std::ostream &s, const raven::Statement &statement) {
    switch (statement.getVariant()) {
        case raven::StatementType::Block:
            return s << static_cast<raven::Block>(statement);
        case raven::StatementType::Expression:
            return s << static_cast<raven::Expression>(statement) << ";";
        case raven::StatementType::Declaration:
            return displayDeclaration(s, statement);
        case raven::StatementType::If:
            return s << static_cast<raven::If>(statement);
        case raven::StatementType::Loop:
            return s << static_cast<raven::While>(statement);
        case raven::StatementType::Return:
            return s << static_cast<raven::Return>(statement) << ";";
    }
    return s;
}
