/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Declaration
*/

#pragma once

#include <memory>

#include "behavior/DeclarationBehavior.hpp"

namespace raven {

    enum class DeclarationType {
        Variable,
        Function
    };

    class Declaration {
        std::shared_ptr<DeclarationBehavior> _behavior;
        DeclarationType _type;

        public:
            Declaration() = default;

            template<typename T>
            requires std::derived_from<std::remove_cvref_t<T>, DeclarationBehavior>
            Declaration(T &&args) {
                _behavior = std::make_shared<std::remove_cvref_t<T>>(std::forward<T>(args));
                if constexpr (std::is_same_v<std::remove_cvref_t<T>, raven::VariableDeclarationBehavior>)
                    _type = DeclarationType::Variable;
                else if constexpr (std::is_same_v<std::remove_cvref_t<T>, raven::FunctionDeclarationBehavior>)
                    _type = DeclarationType::Function;
            }

            ~Declaration() = default;

            template<typename T>
            requires std::derived_from<std::remove_cvref_t<T>, DeclarationBehavior>
            operator T() {
                return *dynamic_cast<T*>(_behavior.get());
            }

            DeclarationType getDeclarationType() const;
            const DeclarationBehavior &getDeclaration() const;

    };

}

std::ostream &operator<<(std::ostream &s, const raven::Declaration &decl);
