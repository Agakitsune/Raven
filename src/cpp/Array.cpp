/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Array
*/

#include "Array.hpp"
#include "Expression.hpp"
#include "Unserializer.hpp"

namespace raven {

    void Array::unserializeArray(Input &input) {
        uint8_t n = unserialize<uint8_t>(input);
        for (uint8_t i = 0; i < n; i++)
            _exps.push_back(std::make_shared<Expression>(unserialize<Expression>(input)));
    }

    const std::vector<std::shared_ptr<Expression>> &Array::getExpressions() const {
        return _exps;
    }

}

std::ostream &operator<<(std::ostream &s, const raven::Array &array) {
    s << "[";
    auto &ref = array.getExpressions();
    for (auto it = ref.begin(); it != ref.end(); ++it) {
        s << *(*it);
        if (it + 1 != ref.end())
            s << ", ";
    }
    s << "]";
    return s;
}
