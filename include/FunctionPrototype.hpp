/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** FunctionPrototype
*/

#pragma once

#include <string>
#include <vector>

#include "behavior/DeclarationBehavior.hpp"

namespace raven {

    class FunctionPrototype {
        std::string _type;
        std::string _name;
        std::vector<std::string> _args;

        public:
            FunctionPrototype() = default;
            FunctionPrototype(const std::string &type, const std::string &name, const std::initializer_list<std::string> &args);
            FunctionPrototype(const std::string &type, const std::string &name, const std::vector<std::string> &args);
            FunctionPrototype(const FunctionDeclarationBehavior &behavior);
            FunctionPrototype(const FunctionPrototype &other) = default;
            ~FunctionPrototype() = default;

            const std::string &getType() const;
            const std::string &getName() const;
            const std::vector<std::string> &getArgs() const;

    };

}
