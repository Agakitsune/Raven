/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** FunctionPrototype
*/

#include "FunctionPrototype.hpp"

namespace raven {

    FunctionPrototype::FunctionPrototype(const std::string &type, const std::string &name, const std::initializer_list<std::string> &args) : _type(type), _name(name), _args(args) {}

    FunctionPrototype::FunctionPrototype(const std::string &type, const std::string &name, const std::vector<std::string> &args) : _type(type), _name(name), _args(args) {}

    FunctionPrototype::FunctionPrototype(const FunctionDeclarationBehavior &behavior) {
        _type = behavior.getType();
        _name = behavior.getName();
        for (const auto &arg : behavior.getArgs())
            _args.push_back(std::get<0>(arg));
    }

    const std::string &FunctionPrototype::getType() const {
        return _type;
    }

    const std::string &FunctionPrototype::getName() const {
        return _name;
    }

    const std::vector<std::string> &FunctionPrototype::getArgs() const {
        return _args;
    }

}
