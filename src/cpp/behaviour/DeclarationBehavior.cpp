/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** DeclarationBehavior
*/

#include "behavior/DeclarationBehavior.hpp"
#include "Unserializer.hpp"

namespace raven {

    void VariableDeclarationBehavior::unserializeDecl(Input &input) {
        _type = unserialize<std::string>(input);
        _name = unserialize<std::string>(input);
        uint8_t _decl = unserialize<uint8_t>(input);
        if (_decl) {
            _declarator = Expression::order(unserialize<Expression>(input));
        }
    }

    const std::string &VariableDeclarationBehavior::getType() const {
        return _type;
    }

    const std::string &VariableDeclarationBehavior::getName() const {
        return _name;
    }

    bool VariableDeclarationBehavior::hasDeclarator() const {
        return _declarator.has_value();
    }

    const Expression &VariableDeclarationBehavior::getExpression() const {
        return _declarator.value();
    }



    void FunctionDeclarationBehavior::unserializeDecl(Input &input) {
        _type = unserialize<std::string>(input);
        _name = unserialize<std::string>(input);
        uint8_t n = unserialize<uint8_t>(input);
        for (size_t i = 0; i < n; ++i) {
            std::string atype = unserialize<std::string>(input);
            std::string aname = unserialize<std::string>(input);
            _args.push_back(std::make_tuple(atype, aname));
        }
        _blk.unserializeBlk(input);
    }

    const std::string &FunctionDeclarationBehavior::getType() const {
        return _type;
    }

    const std::string &FunctionDeclarationBehavior::getName() const {
        return _name;
    }

    const std::vector<std::tuple<std::string, std::string>> &FunctionDeclarationBehavior::getArgs() const {
        return _args;
    }

    const Block &FunctionDeclarationBehavior::getBlock() const {
        return _blk;
    }

}

std::ostream &operator<<(std::ostream &s, const raven::DeclarationBehavior &behavior) {
    const raven::VariableDeclarationBehavior *ptr = dynamic_cast<const raven::VariableDeclarationBehavior*>(&behavior);
    if (ptr != nullptr)
        return s << *ptr;
    const raven::FunctionDeclarationBehavior *fPtr = dynamic_cast<const raven::FunctionDeclarationBehavior*>(&behavior);
    if (fPtr != nullptr)
        return s << *fPtr;
    return s;
}

std::ostream &operator<<(std::ostream &s, const raven::VariableDeclarationBehavior &behavior) {
    s << behavior.getType() << " " << behavior.getName();
    if (behavior.hasDeclarator())
        s << " = " << behavior.getExpression();
    return s;
}

std::ostream &operator<<(std::ostream &s, const raven::FunctionDeclarationBehavior &behavior) {
    s << behavior.getType() << " " << behavior.getName() << "(";
    auto &ref = behavior.getArgs();
    for (auto it = ref.begin(); it != ref.end(); ++it) {
        s << std::get<0>(*it) << " " << std::get<1>(*it);
        if (it + 1 != ref.end())
            s << ", ";
    }
    return s << ") " << behavior.getBlock();
}
