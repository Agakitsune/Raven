/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Block
*/

#include "Block.hpp"
#include "Statement.hpp"
#include "Unserializer.hpp"
#include "exception/SerializationException.hpp"

namespace raven {

    void Block::unserializeBlk(Input &input) {
        if (*input.getData() != 10)
            throw exception::SerializationException("Expected Block begin");
        input.consume(1);
        while (*input.getData() != 10)
            _statements.push_back(std::make_shared<Statement>(unserialize<Statement>(input)));
        input.consume(1);
    }

    const std::vector<std::shared_ptr<Statement>> &Block::getStatements() const {
        return _statements;
    }

}

std::ostream &operator<<(std::ostream &s, const raven::Block &blk) {
    s << "{" << std::endl;
    for (const std::shared_ptr<raven::Statement> &st : blk.getStatements())
        s << "\t" << *st << std::endl;
    return s << "}";
}
