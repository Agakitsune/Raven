/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Call
*/

#include "Call.hpp"
#include "Expression.hpp"
#include "Unserializer.hpp"

namespace raven {

    void Call::unserializeCall(Input &input) {
        _input = unserialize<std::string>(input);
        uint8_t n = unserialize<uint8_t>(input);
        for (size_t i = 0; i < n; ++i)
            _args.push_back(std::make_shared<Expression>(Expression::order(unserialize<Expression>(input))));
    }

    const std::string &Call::getName() const {
        return _input;
    }

    const std::vector<std::shared_ptr<Expression>> &Call::getArgs() const {
        return _args;
    }

}

std::ostream &operator<<(std::ostream &s, const raven::Call &call) {
    s << call.getName() << "(";
    auto &ref = call.getArgs();
    for (auto it = ref.begin(); it != ref.end(); it++) {
        s << *(it->get());
        if (it + 1 != ref.end())
            s << ", ";
    }
    return s << ")";
}
