/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Unserializer
*/

#include <bit>

#include "Unserializer.hpp"

namespace raven {

    template<>
    std::string unserialize(Input &input) {
        std::string str(reinterpret_cast<char*>(input.getData()));
        input.consume(str.size() + 1);
        return str;
    }

    template<>
    long unserialize(Input &input) {
        uint8_t *data = input.getData();
        if (std::endian::native == std::endian::little)
            for (size_t i = 0; i < 4; ++i) {
                uint8_t hold = data[i];
                data[i] = data[7 - i];
                data[7 - i] = hold;
            }
        long value = *reinterpret_cast<long*>(data);
        input.consume(sizeof(long));
        return value;
    }

    template<>
    double unserialize(Input &input) {
        uint8_t *data = input.getData();
        if (std::endian::native == std::endian::little)
            for (size_t i = 0; i < 4; ++i) {
                uint8_t hold = data[i];
                data[i] = data[7 - i];
                data[7 - i] = hold;
            }
        double value = *reinterpret_cast<double*>(data);
        input.consume(sizeof(double));
        return value;
    }

    template<>
    Literal unserialize(Input &input) {
        uint8_t type = unserialize<uint8_t>(input);
        Literal literal;
        switch (type) {
            case 0:
                NumberLiteralBehavior::unserializeliteral(literal, input);
                break;
            case 1:
                StringLiteralBehavior::unserializeliteral(literal, input);
                break;
            case 2:
                BooleanLiteralBehavior::unserializeliteral(literal, input);
                break;
            case 3:
                CharacterLiteralBehavior::unserializeliteral(literal, input);
                break;
            case 4:
                NullLiteralBehavior::unserializeliteral(literal, input);
                break;
            default:
                throw exception::SerializationException("Invalid literal type");
        }
        return literal;
    }

    template<>
    Identifier unserialize(Input &input) {
        Identifier id;
        id.unserializeId(input);
        return id;
    }

    template<>
    Operation unserialize(Input &input) {
        Operation op;
        op.unserializeOp(input);
        return op;
    }

    template<>
    Call unserialize(Input &input) {
        Call call;
        call.unserializeCall(input);
        return call;
    }

    template<>
    Fold unserialize(Input &input) {
        Fold fold;
        fold.unserializeFold(input);
        return fold;
    }

    template<>
    Index unserialize(Input &input) {
        Index index;
        index.unserializeIndex(input);
        return index;
    }

    template<>
    Array unserialize(Input &input) {
        Array arr;
        arr.unserializeArray(input);
        return arr;
    }

    template<>
    Expression unserialize(Input &input) {
        uint8_t type = unserialize<uint8_t>(input);
        Expression exp;
        switch (type) {
            case 0:
                exp.setValue(unserialize<Literal>(input));
                break;
            case 1:
                exp.setValue(unserialize<Identifier>(input));
                break;
            case 2:
                exp.setValue(unserialize<Operation>(input));
                break;
            case 3:
                exp.setValue(unserialize<Call>(input));
                break;
            case 4:
                exp.setValue(unserialize<Fold>(input));
                break;
            case 5:
                exp.setValue(unserialize<Index>(input));
                break;
            case 6:
                exp.setValue(unserialize<Array>(input));
                break;
            default:
                throw exception::SerializationException("Invalid expression type");
        }
        return exp;
    }

    template<>
    Block unserialize(Input &input) {
        Block blk;
        blk.unserializeBlk(input);
        return blk;
    }

    template<>
    Declaration unserialize(Input &input) {
        uint8_t type = unserialize<uint8_t>(input);
        std::unique_ptr<DeclarationBehavior> behavior = nullptr;
        switch (type) {
            case 0:
                behavior = std::make_unique<VariableDeclarationBehavior>();
                behavior->unserializeDecl(input);
                return Declaration(dynamic_cast<VariableDeclarationBehavior&>(*behavior));
            case 1:
                behavior = std::make_unique<FunctionDeclarationBehavior>();
                behavior->unserializeDecl(input);
                return Declaration(dynamic_cast<FunctionDeclarationBehavior&>(*behavior));
        }
        return Declaration();
    }

    template<>
    If unserialize(Input &input) {
        If i;
        i.unserializeIf(input);
        return i;
    }

    template<>
    While unserialize(Input &input) {
        While w;
        w.unserializeWhile(input);
        return w;
    }

    template<>
    Return unserialize(Input &input) {
        Return ret;
        ret.unserializeRet(input);
        return ret;
    }

    template<>
    Statement unserialize(Input &input) {
        uint8_t type = unserialize<uint8_t>(input);
        Statement exp;
        switch (type) {
            case 0:
                exp.setValue(unserialize<Block>(input));
                break;
            case 1:
                exp.setValue(unserialize<Expression>(input));
                break;
            case 2:
                exp.setValue(unserialize<Declaration>(input));
                break;
            case 3:
                exp.setValue(unserialize<If>(input));
                break;
            case 4:
                exp.setValue(unserialize<While>(input));
                break;
            case 5:
                exp.setValue(unserialize<Return>(input));
                break;
            default:
                throw exception::SerializationException("Invalid statement type");
        }
        return exp;
    }

}
