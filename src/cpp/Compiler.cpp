/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Compiler
*/

#include <iostream>

#include "Compiler.hpp"

namespace raven {

    std::string generateKey(const std::filesystem::path &scope, const std::string &name) {
        return std::string(scope / name);
    }

    std::string getArrayType(const std::string &type) {
        return type + "[]";
    }

    std::string getArrayValueType(const std::string &type) {
        if (type == "String")
            return "Character";
        if (type.ends_with("[]"))
            return type.substr(0, type.size() - 2);
        throw exception::CompilerException("Type '" + type + "' is not an array");
    }

    std::string getGlobalExpressionType(const Expression &exp, const std::string &type) {
        Literal lit;
        Identifier id;
        Index index;
        Array array;
        switch (exp.getVariant()) {
            case ExpressionType::ExpLiteral:
                lit = static_cast<Literal>(exp);
                switch (lit.getVariant()) {
                    case LiteralType::Boolean:
                        return "Boolean";
                    case LiteralType::Character:
                        return "Character";
                    case LiteralType::Floating:
                        return "Floating";
                    case LiteralType::Integral:
                        return "Integer";
                    case LiteralType::String:
                        return "String";
                }
                break;
            case ExpressionType::ExpIdentifier:
                id = static_cast<Identifier>(exp);
                return Compiler::getVariable(id.getId()).type;
            case ExpressionType::ExpIndex:
                index = static_cast<Index>(exp);
                return getArrayValueType(Compiler::getVariable(index.getName()).type);
            case ExpressionType::ExpCall:
                return Compiler::getFunction(static_cast<Call>(exp).getName()).getType();
            case ExpressionType::ExpFold:
                return getGlobalExpressionType(static_cast<Fold>(exp).getExpression());
            case ExpressionType::ExpArray:
                array = static_cast<Array>(exp);
                if (array.getExpressions().empty())
                    return type;
                return getArrayType(getGlobalExpressionType(*static_cast<Array>(exp).getExpressions().at(0), getArrayValueType(type)));
        }
        return "";
    }

    const std::pair<size_t, ASM::RegisterSize> &getMetaType(const std::string &type) {
        static std::unordered_map<std::string, std::pair<size_t, ASM::RegisterSize>> sizeofs = {
            {"Character", {1, ASM::RegisterSize::S8}},
            {"Boolean", {1, ASM::RegisterSize::S8}},
            {"Short", {2, ASM::RegisterSize::S16}},
            {"Integer", {4, ASM::RegisterSize::S32}},
            {"Floating", {4, ASM::RegisterSize::S32}},
            {"Double", {8, ASM::RegisterSize::S64}},
            {"Long", {8, ASM::RegisterSize::S64}},
            {"String", {8, ASM::RegisterSize::S64}}
        };
        if (type.ends_with("[]"))
            return sizeofs["String"];
        if (sizeofs.find(type) == sizeofs.end())
            throw exception::CompilerException("Type '" + type + "' not found");
        return sizeofs[type];
    }

    size_t sizeofType(const std::string &type) {
        return getMetaType(type).first;
    }

    ASM::AccessFactor getAccessFactor(const std::string &type) {
        const auto &meta = getMetaType(type);
        switch (meta.second) {
            case ASM::RegisterSize::S8:
                return ASM::AccessFactor::Factor1;
            case ASM::RegisterSize::S16:
                return ASM::AccessFactor::Factor2;
            case ASM::RegisterSize::S32:
                return ASM::AccessFactor::Factor4;
            case ASM::RegisterSize::S64:
                return ASM::AccessFactor::Factor8;
        }
        return ASM::AccessFactor::Factor1;
    }

    ASM::RegisterSize getRegisterSize(const std::string &type) {
        return getMetaType(type).second;
    }

    void dumpExpression(const std::string &name,
                        const std::string &type,
                        const Expression &exp,
                        std::vector<uint8_t> &data,
                        size_t index
    ) {
        Literal lit;
        Fold fold;
        Array array;
        Index idx;
        Identifier id;
        Call call;

        size_t size = sizeofType(type);
        uint8_t tmp[8] = {0};
        std::wstring_convert<raven::codecvtUTF8, char32_t> conv;
        std::string str;
        std::string arrayValueType;
        long longValue;

        switch (exp.getVariant()) {
            case ExpressionType::ExpLiteral:
                lit = static_cast<Literal>(exp);
                switch (lit.getVariant()) {
                    case LiteralType::Integral:
                        longValue = static_cast<long>(lit);
                        std::memcpy(tmp, &longValue, 8);
                        for (int i = 0; i < size; ++i) {
                            data.push_back(tmp[i]);
                        }
                        break;
                    case LiteralType::Character:
                        data.push_back(conv.to_bytes(static_cast<char32_t>(lit)).at(0));
                        break;
                    case LiteralType::Boolean:
                        data.push_back(static_cast<bool>(lit));
                        break;
                    case LiteralType::Null:
                        for (int i = 0; i < size; ++i) {
                            data.push_back(0);
                        }
                        break;
                    case LiteralType::Floating:
                        throw exception::CompilerException("Floating value not supported");
                    case LiteralType::String:
                        str = static_cast<std::string>(lit);
                        Compiler::addDependency(Dependency(RodataDependency, name, str, index));
                        for (int i = 0; i < 8; ++i) {
                            data.push_back(0);
                        }
                        break;
                }
                break;
            case ExpressionType::ExpFold:
                dumpExpression(name, type, static_cast<Fold>(exp).getExpression(), data);
                break;
            case ExpressionType::ExpArray:
                array = static_cast<Array>(exp);
                arrayValueType = getArrayValueType(type);
                for (auto &it : array.getExpressions()) {
                    if (arrayValueType != getGlobalExpressionType(*it, getArrayValueType(type)))
                        throw exception::CompilerException("Array type mismatch");
                    dumpExpression(name, arrayValueType, *it, data, index + 1);
                }
                break;
            case ExpressionType::ExpOperation:
                break;
            case ExpressionType::ExpIdentifier:
                id = static_cast<Identifier>(exp);
                if (Compiler::getVariable(id.getId()).type != type)
                    throw exception::CompilerException("Type mismatch");
                Compiler::addDependency(Dependency(VariableDependency, name, id.getId(), index));
                for (int i = 0; i < size; ++i) {
                    data.push_back(0);
                }
                break;
            case ExpressionType::ExpIndex:
                idx = static_cast<Index>(exp);
                if (idx.getExpression().getVariant() != ExpressionType::ExpLiteral)
                    throw exception::CompilerException("Could not resolve expression dependency, index must be a literal");
                lit = static_cast<Literal>(idx.getExpression());
                if (lit.getVariant() != LiteralType::Integral)
                    throw exception::CompilerException("Could not dump expression, index must be an integer");
                Compiler::addDependency(Dependency(VariableDependency, name, idx.getName(), index, static_cast<long>(lit)));
                for (int i = 0; i < size; ++i) {
                    data.push_back(0);
                }
                break;
            case ExpressionType::ExpCall:
                call = static_cast<Call>(exp);
                if (Compiler::getFunction(call.getName()).getType() != type)
                    throw exception::CompilerException("Type mismatch");
                Compiler::addDependency(Dependency(FunctionDependency, name, static_cast<Call>(exp).getName(), index));
                for (int i = 0; i < size; ++i) {
                    data.push_back(0);
                }
                break;
        }
    }

    Variable &FunctionCompiler::getVariable(const std::string &name) {
        std::filesystem::path p = scope;
        while (true) {
            std::string key = generateKey(p, name);
            if (variables.find(key) == variables.end()) {
                if (p == "root") {
                    return Compiler::getVariable(name);
                }
                p = p.parent_path();
            } else {
                return variables.at(key);
            }
        }
    }

    bool FunctionCompiler::hasVariable(const std::string &name) {
        std::filesystem::path p = scope;
        while (true) {
            std::string key = generateKey(p, name);
            if (variables.find(key) == variables.end()) {
                if (p == "root") {
                    return Compiler::hasVariable(name);
                }
                p = p.parent_path();
            } else {
                return true;
            }
        }
    }

    std::string FunctionCompiler::getVariableType(const std::string &name) {
        return getVariable(name).type;
    }

    std::string FunctionCompiler::getExpressionType(const Expression &exp) {
        Literal lit;
        Identifier id;
        Index index;
        switch (exp.getVariant()) {
            case ExpressionType::ExpLiteral:
                lit = static_cast<Literal>(exp);
                switch (lit.getVariant()) {
                    case LiteralType::Integral:
                        return "Integer";
                    case LiteralType::Character:
                        return "Character";
                    case LiteralType::Floating:
                        return "Double";
                    case LiteralType::String:
                        return "String";
                    case LiteralType::Boolean:
                        return "Boolean";
                }
                break;
            case ExpressionType::ExpIdentifier:
                id = static_cast<Identifier>(exp);
                return getVariableType(id.getId());
            case ExpressionType::ExpIndex:
                index = static_cast<Index>(exp);
                return getArrayValueType(getVariableType(index.getName()));
            case ExpressionType::ExpCall:
                return Compiler::getFunction(static_cast<Call>(exp).getName()).getType();
            case ExpressionType::ExpOperation:
                return "Integer";
            case ExpressionType::ExpArray:
                if (static_cast<Array>(exp).getExpressions().empty()) {
                    log::warn << "Empty array, type cannot be determined" << log::endl;
                    return "";
                }
                return getArrayType(getExpressionType(*static_cast<Array>(exp).getExpressions().at(0)));
        }
        return "";
    }

    bool FunctionCompiler::isIntegralType(const std::string &type) {
        return type == "Long" || type == "Integer" || type == "Short" || type == "Character" || type == "Boolean";
    }

    bool FunctionCompiler::needPromotion(ASM::Register reg, const std::string &type) {
        if (!isIntegralType(type))
            throw exception::CompilerException("Cannot promote non-integral type");
        return reg.size > getRegisterSize(type);
    }

    void FunctionCompiler::destroyScope(const std::filesystem::path &scope) {
        std::vector<std::string> toDelete;
        for (auto &it : variables) {
            if (it.first.starts_with(std::string(scope))) {
                toDelete.push_back(it.first);
            }
        }
        for (auto &it : toDelete) {
            variables.erase(it);
        }
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::loadVariable(const std::string &name, ASM::Register reg) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;

        ASM::Register copy = reg;

        auto mov = std::make_shared<ASM::Mov>();
        Variable &var = getVariable(name);
        int64_t diff = var.offset;

        reg.size = getRegisterSize(var.type);

        if (var.outOfScope) {
            mov->setDst(reg).setSrc(ASM::Access().setValue(ASM::Immediate(var.offset).setSize(ASM::IMM32)));
            Compiler::addVariable(name, [=](size_t offset){
                mov->setDst(reg).setSrc(ASM::Access().setValue(ASM::Immediate(offset)));
            });
        } else {
            mov->setDst(reg).setSrc(ASM::Access().setValue(ASM::RBP).setOffset(ASM::Immediate(-diff)));
        }
        instructions.push_back(mov);
        functionSize += mov->getSize();

        if (isIntegralType(var.type) && needPromotion(copy, var.type)) {
            auto prom = ASM::Move::move(copy, reg);
            instructions.insert(instructions.end(), prom.begin(), prom.end());
            for (auto &it : prom) {
                functionSize += it->getSize();
            }
        }

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::storeVariable(const std::string &name, ASM::Register reg) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;

        auto mov = std::make_shared<ASM::Mov>();
        Variable &var = getVariable(name);
        int64_t diff = var.offset;

        reg.size = getRegisterSize(var.type);

        if (var.outOfScope) {
            mov->setDst(ASM::Access().setValue(ASM::Immediate(var.offset).setSize(ASM::IMM32))).setSrc(reg);
            Compiler::addVariable(name, [=](size_t offset){
                mov->setDst(ASM::Access().setValue(ASM::Immediate(offset))).setSrc(reg);
            });
        } else {
            mov->setDst(ASM::Access().setValue(ASM::RBP).setOffset(ASM::Immediate(-diff))).setSrc(reg);
        }
        instructions.push_back(mov);
        functionSize += mov->getSize();

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::createVariable(const std::string &type, const std::string &name, ASM::Register reg) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        offset += sizeofType(type);
        int64_t diff = offset;

        reg.size = getRegisterSize(type);

        variables[generateKey(scope, name)] = Variable{type, offset, nullptr, false};

        auto mov = std::make_shared<ASM::Mov>();
        mov->setDst(ASM::Access().setValue(ASM::RBP).setOffset(ASM::Immediate(-diff))).setSrc(reg);
        instructions.push_back(mov);

        functionSize += mov->getSize();

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::loadIndex(const Index &index, ASM::Register reg) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;

        ASM::Register copy = reg;

        std::string type = getArrayValueType(getVariableType(index.getName()));
        reg.size = getRegisterSize(type);

        auto tmp = loadVariable(index.getName(), ASM::R10);
        instructions.insert(instructions.end(), tmp.begin(), tmp.end());

        const Expression &idx = index.getExpression();

        if (getExpressionType(idx) == "Float")
            throw exception::CompilerException("Index cannot be a Float");
        if (getExpressionType(idx) == "Double")
            throw exception::CompilerException("Index cannot ba a Double");
        if (getExpressionType(idx) == "String")
            throw exception::CompilerException("Index cannot be a String");
        tmp = loadExpression(idx, ASM::R11);
        instructions.insert(instructions.end(), tmp.begin(), tmp.end());

        ASM::AccessFactor fac = getAccessFactor(getVariableType(index.getName()));

        auto mov = std::make_shared<ASM::Mov>();
        mov->setDst(reg).setSrc(ASM::Access().setValue(ASM::R10).setOffset(ASM::R11).setFactor(fac));
        instructions.push_back(mov);

        functionSize += mov->getSize();

        if (isIntegralType(type) && needPromotion(copy, type)) {
            auto prom = ASM::Move::move(copy, reg);
            instructions.insert(instructions.end(), prom.begin(), prom.end());
            for (auto &it : prom) {
                functionSize += it->getSize();
            }
        }

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::storeIndex(const Index &index, ASM::Register reg) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;

        reg.size = getRegisterSize(getArrayValueType(getVariableType(index.getName())));

        auto tmp = loadVariable(index.getName(), ASM::R10);
        instructions.insert(instructions.end(), tmp.begin(), tmp.end());

        const Expression &idx = index.getExpression();

        if (getExpressionType(idx) == "Float")
            throw exception::CompilerException("Index cannot be a Float");
        if (getExpressionType(idx) == "Double")
            throw exception::CompilerException("Index cannot ba a Double");
        if (getExpressionType(idx) == "String")
            throw exception::CompilerException("Index cannot be a String");
        tmp = loadExpression(idx, ASM::R11);
        instructions.insert(instructions.end(), tmp.begin(), tmp.end());

        ASM::AccessFactor fac = getAccessFactor(getVariableType(index.getName()));

        auto mov = std::make_shared<ASM::Mov>();
        mov->setSrc(reg).setDst(ASM::Access().setValue(ASM::R10).setOffset(ASM::R11).setFactor(fac));
        instructions.push_back(mov);

        functionSize += mov->getSize();

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::loadUpdate(const Expression &exp, ASM::Register reg, bool inc) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        std::vector<std::shared_ptr<ASM::Instruction>> tmp;

        Operation op;
        Index idx;
        Identifier id;
        Fold fold;

        switch (exp.getVariant()) {
            case ExpressionType::ExpLiteral:
            case ExpressionType::ExpArray:
            case ExpressionType::ExpCall:
                throw exception::CompilerException("Cannot update rvalue expression");
            case ExpressionType::ExpOperation:
                op = static_cast<Operation>(exp);
                switch (op.getType()) {
                    case raven::OperationType::IncrementPre:
                    case raven::OperationType::IncrementPost:
                        tmp = loadUpdate(op.getFirstExpression(), reg, true);
                        instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                        break;
                    case raven::OperationType::DecrementPre:
                    case raven::OperationType::DecrementPost:
                        tmp = loadUpdate(op.getFirstExpression(), reg, false);
                        instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                        break;
                    default:
                        throw exception::CompilerException("Cannot update rvalue expression");
                }
                break;
            case ExpressionType::ExpIndex:
                idx = static_cast<Index>(exp);
                reg.size = getRegisterSize(getArrayValueType(getVariableType(id.getId())));
                tmp = loadIndex(idx, reg);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                if (inc) {
                    auto update = std::make_shared<ASM::Inc>();
                    update->setReg(reg);
                    instructions.push_back(update);
                    functionSize += update->getSize();
                } else {
                    auto update = std::make_shared<ASM::Dec>();
                    update->setReg(reg);
                    instructions.push_back(update);
                    functionSize += update->getSize();
                }
                tmp = storeIndex(idx, reg);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                return instructions;
            case ExpressionType::ExpIdentifier:
                id = static_cast<Identifier>(exp);
                reg.size = getRegisterSize(getVariableType(id.getId()));
                tmp = loadVariable(id.getId(), reg);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                if (inc) {
                    auto update = std::make_shared<ASM::Inc>();
                    update->setReg(reg);
                    instructions.push_back(update);
                    functionSize += update->getSize();
                } else {
                    auto update = std::make_shared<ASM::Dec>();
                    update->setReg(reg);
                    instructions.push_back(update);
                    functionSize += update->getSize();
                }
                tmp = storeVariable(id.getId(), reg);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                return instructions;
            case ExpressionType::ExpFold:
                throw exception::CompilerException("Cannot update rvalue expression");
        }

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::generateOperation(const Operation &op,
                                                                    ASM::Register reg,
                                                                    const std::function<std::vector<std::shared_ptr<ASM::Instruction>>(ASM::Register, ASM::Register)> &classic,
                                                                    const std::function<std::vector<std::shared_ptr<ASM::Instruction>>(ASM::Register, const ASM::Immediate&)> &variant,
                                                                    const std::function<long(long, long)> &optimize,
                                                                    bool ignoreOptimize)
    {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        std::vector<std::shared_ptr<ASM::Instruction>> tmp;
        bool needPush = false;

        ASM::Register extend = reg;
        extend.size = ASM::RegisterSize::S64;

        ASM::Register dest = reg;
        if (reg.extend)
            ++dest.reg;
        else
            dest.reg = 0;
        dest.extend = true;
        dest.useREX = true;

        if (dest.reg > 6) {
            dest.reg = 0;
            needPush = reg.extend;
        }

        Literal lit1;
        Literal lit2;

        long longValue1;
        long longValue2;
        long longValue;

        const Expression &exp1 = op.getFirstExpression();
        const Expression &exp2 = op.getSecondExpression();

        if (!isIntegralType(getExpressionType(exp1)) || !isIntegralType(getExpressionType(exp2)))
            throw exception::CompilerException("Cannot perform operation on non-integral types");

        if (exp1.getVariant() == ExpressionType::ExpLiteral && exp2.getVariant() == ExpressionType::ExpLiteral) {
            lit1 = static_cast<Literal>(exp1);
            lit2 = static_cast<Literal>(exp2);
            longValue1 = static_cast<long>(lit1);
            longValue2 = static_cast<long>(lit2);
            longValue = optimize(longValue1, longValue2);
            if (!ignoreOptimize) {
                auto mov = std::make_shared<ASM::Mov>(reg, ASM::Immediate(longValue));
                instructions.push_back(mov);
                functionSize += mov->getSize();
            }
        } else if (exp2.getVariant() == ExpressionType::ExpLiteral) {
            lit2 = static_cast<Literal>(exp2);
            if (lit2.getVariant() == LiteralType::Integral) {
                longValue2 = static_cast<long>(lit2);
                tmp = loadExpression(exp1, reg);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                tmp = variant(reg, ASM::Immediate(longValue2));
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                for (auto &it : tmp) {
                    functionSize += it->getSize();
                }
            } else {
                throw exception::CompilerException("Cannot perform operation on non-integral literals");
            }
        } else if (exp2.getVariant() == ExpressionType::ExpOperation) {
            if (needPush) {
                auto save = std::make_shared<ASM::Push>(extend);
                instructions.push_back(save);
                functionSize += save->getSize();

                tmp = loadExpression(exp2, extend);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());

                auto move = ASM::Move::safeMove(dest, extend);
                instructions.insert(instructions.end(), move.begin(), move.end());
                for (auto &it : move) {
                    functionSize += it->getSize();
                }

                auto restore = std::make_shared<ASM::Pop>(extend);
                instructions.push_back(restore);
                functionSize += restore->getSize();
            } else {
                tmp = loadExpression(exp2, dest);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
            }
            if (exp1.getVariant() == ExpressionType::ExpCall) {
                // to avoid dest being overwritten by the call

                auto push = std::make_shared<ASM::Push>(dest);
                instructions.push_back(push);
                functionSize += push->getSize();
                tmp = loadExpression(exp1, reg);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                auto pop = std::make_shared<ASM::Pop>(dest);
                instructions.push_back(pop);
                functionSize += pop->getSize();
            } else {
                tmp = loadExpression(exp1, reg);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
            }
            tmp = classic(reg, dest);
            instructions.insert(instructions.end(), tmp.begin(), tmp.end());
            for (auto &it : tmp) {
                functionSize += it->getSize();
            }
        } else {
            tmp = loadExpression(exp1, reg);
            instructions.insert(instructions.end(), tmp.begin(), tmp.end());
            if (exp2.getVariant() == ExpressionType::ExpCall) {
                // to avoid reg being overwritten by the call

                auto push = std::make_shared<ASM::Push>(extend);
                instructions.push_back(push);
                functionSize += push->getSize();

                tmp = loadExpression(exp2, dest);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());

                auto pop = std::make_shared<ASM::Pop>(extend);
                instructions.push_back(pop);
                functionSize += pop->getSize();
            } else {
                tmp = loadExpression(exp2, dest);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
            }
            std::cout << "reg: " << (int)reg.reg << std::endl;
            std::cout << "extend dest: " << (int)dest.extend << std::endl;
            tmp = classic(reg, dest);
            instructions.insert(instructions.end(), tmp.begin(), tmp.end());
            for (auto &it : tmp) {
                functionSize += it->getSize();
            }
        }

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::loadOperation(const Operation &op, ASM::Register reg) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        std::vector<std::shared_ptr<ASM::Instruction>> tmp;

        ASM::Register copy = reg;

        Literal lit1;
        Literal lit2;

        switch (op.getType()) {
            case OperationType::UnaryBitwiseNot:
                tmp = loadExpression(op.getFirstExpression(), reg);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                instructions.push_back(std::make_shared<ASM::Not>(reg));
                functionSize += instructions.back()->getSize();
                break;
            case OperationType::UnaryArithmeticMinus:
                tmp = loadExpression(op.getFirstExpression(), reg);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                instructions.push_back(std::make_shared<ASM::Neg>(reg));
                functionSize += instructions.back()->getSize();
                break;
            case OperationType::UnaryLogicalNot:
                tmp = logicExpression(op.getFirstExpression(), reg, true);
                break;
            case OperationType::IncrementPre:
            case OperationType::IncrementPost:
                return loadUpdate(op.getFirstExpression(), reg, true);
            case OperationType::DecrementPre:
            case OperationType::DecrementPost:
                return loadUpdate(op.getFirstExpression(), reg, false);
            case OperationType::BinaryArithmeticAdd:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        std::vector<std::shared_ptr<ASM::Instruction>> inst = {std::make_shared<ASM::Add>(reg1, reg2)};
                        return inst;
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        std::vector<std::shared_ptr<ASM::Instruction>> inst = {std::make_shared<ASM::Add>(reg1, imm)};
                        return inst;
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 + longValue2;
                    }
                );
            case OperationType::BinaryArithmeticMinus:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        std::vector<std::shared_ptr<ASM::Instruction>> inst = {std::make_shared<ASM::Sub>(reg1, reg2)};
                        return inst;
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        std::vector<std::shared_ptr<ASM::Instruction>> inst = {std::make_shared<ASM::Sub>(reg1, imm)};
                        return inst;
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 - longValue2;
                    }
                );
            case OperationType::BinaryArithmeticMultiply:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        return ASM::IMul::mul(reg1, reg2);
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        return ASM::IMul::mul(reg1, imm.getValue());
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 * longValue2;
                    }
                );
            case OperationType::BinaryArithmeticDivide:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        return ASM::IDiv::div(reg1, reg2);
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        return ASM::IDiv::div(reg1, imm.getValue());
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 / longValue2;
                    }
                );
            case OperationType::BinaryArithmeticModulo:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        return ASM::IDiv::mod(reg1, reg2);
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        return ASM::IDiv::mod(reg1, imm.getValue());
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 % longValue2;
                    }
                );
            case OperationType::BinaryBitwiseAnd:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        std::vector<std::shared_ptr<ASM::Instruction>> inst = {std::make_shared<ASM::And>(reg1, reg2)};
                        return inst;
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        std::vector<std::shared_ptr<ASM::Instruction>> inst = {std::make_shared<ASM::And>(reg1, imm)};
                        return inst;
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 & longValue2;
                    }
                );
            case OperationType::BinaryBitwiseOr:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        std::vector<std::shared_ptr<ASM::Instruction>> inst = {std::make_shared<ASM::Or>(reg1, reg2)};
                        return inst;
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        std::vector<std::shared_ptr<ASM::Instruction>> inst = {std::make_shared<ASM::Or>(reg1, imm)};
                        return inst;
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 | longValue2;
                    }
                );
            case OperationType::BinaryBitwiseXor:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        std::vector<std::shared_ptr<ASM::Instruction>> inst = {std::make_shared<ASM::Xor>(reg1, reg2)};
                        return inst;
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        std::vector<std::shared_ptr<ASM::Instruction>> inst = {std::make_shared<ASM::Xor>(reg1, imm)};
                        return inst;
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 ^ longValue2;
                    }
                );
            case OperationType::BinaryBitwiseLShift:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        return ASM::Shift::sal(reg1, reg2);
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        return ASM::Shift::sal(reg1, imm.getValue());
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 << longValue2;
                    }
                );
            case OperationType::BinaryBitwiseRShift:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        return ASM::Shift::sar(reg1, reg2);
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        return ASM::Shift::sar(reg1, imm.getValue());
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 >> longValue2;
                    }
                );
            case OperationType::BinaryBitwiseLRotShift:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        return ASM::Rotate::rol(reg1, reg2);
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        return ASM::Rotate::rol(reg1, imm.getValue());
                    },
                    [this](long longValue1, long longValue2) {
                        return std::rotl((uint64_t)longValue1, (int)longValue2);
                    }
                );
            case OperationType::BinaryBitwiseRRotShift:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        return ASM::Rotate::ror(reg1, reg2);
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        return ASM::Rotate::ror(reg1, imm.getValue());
                    },
                    [this](long longValue1, long longValue2) {
                        return std::rotr((uint64_t)longValue1, (int)longValue2);
                    }
                );
            case OperationType::BinaryLogicalAnd:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        std::vector<std::shared_ptr<ASM::Instruction>> tmp = logicRegister(reg1, false);
                        inst.insert(inst.end(), tmp.begin(), tmp.end());
                        tmp = logicRegister(reg2, false);
                        inst.insert(inst.end(), tmp.begin(), tmp.end());
                        inst.push_back(std::make_shared<ASM::And>(reg1, reg2));
                        return inst;
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        if (imm.getValue())
                            return inst;
                        inst.push_back(std::make_shared<ASM::Mov>(reg1, 0));
                        return inst;
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 && longValue2;
                    }
                );
            case OperationType::BinaryLogicalOr:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        std::vector<std::shared_ptr<ASM::Instruction>> tmp = logicRegister(reg1, false);
                        inst.insert(inst.end(), tmp.begin(), tmp.end());
                        tmp = logicRegister(reg2, false);
                        inst.insert(inst.end(), tmp.begin(), tmp.end());
                        inst.push_back(std::make_shared<ASM::Or>(reg1, reg2));
                        return inst;
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        if (imm.getValue())
                            inst.push_back(std::make_shared<ASM::Mov>(reg1, 1));
                        return inst;
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 || longValue2;
                    }
                );
            case OperationType::BinaryLogicalXor:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        std::vector<std::shared_ptr<ASM::Instruction>> tmp = logicRegister(reg1, false);
                        inst.insert(inst.end(), tmp.begin(), tmp.end());
                        tmp = logicRegister(reg2, false);
                        inst.insert(inst.end(), tmp.begin(), tmp.end());
                        inst.push_back(std::make_shared<ASM::Xor>(reg1, reg2));
                        return inst;
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        std::vector<std::shared_ptr<ASM::Instruction>> tmp = logicRegister(reg1, false);
                        inst.insert(inst.end(), tmp.begin(), tmp.end());
                        if (imm.getValue())
                            inst.push_back(std::make_shared<ASM::Xor>(reg1, 1));
                        else
                            inst.push_back(std::make_shared<ASM::Xor>(reg1, 0));
                        return inst;
                    },
                    [this](long longValue1, long longValue2) {
                        return (!!longValue1) ^ (!!longValue2);
                    }
                );
            case OperationType::BinaryLogicalEqual:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        ASM::Register copy = reg1;
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        inst.push_back(std::make_shared<ASM::Cmp>(reg1, reg2));
                        inst.push_back(std::make_shared<ASM::Set>(reg1, ASM::Equal));
                        if (reg1.size != ASM::S8) {
                            copy.size = ASM::S8;
                            inst.push_back(std::make_shared<ASM::Move>(reg1, copy, ASM::MoveType::ZeroExtend));
                        }
                        return inst;
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        ASM::Register copy = reg1;
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        inst.push_back(std::make_shared<ASM::Cmp>(reg1, imm));
                        inst.push_back(std::make_shared<ASM::Set>(reg1, ASM::Equal));
                        if (reg1.size != ASM::S8) {
                            copy.size = ASM::S8;
                            inst.push_back(std::make_shared<ASM::Move>(reg1, copy, ASM::MoveType::ZeroExtend));
                        }
                        return inst;
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 == longValue2;
                    }
                );
            case OperationType::BinaryLogicalNotEqual:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        ASM::Register copy = reg1;
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        inst.push_back(std::make_shared<ASM::Cmp>(reg1, reg2));
                        inst.push_back(std::make_shared<ASM::Set>(reg1, ASM::NotEqual));
                        if (reg1.size != ASM::S8) {
                            copy.size = ASM::S8;
                            inst.push_back(std::make_shared<ASM::Move>(reg1, copy, ASM::MoveType::ZeroExtend));
                        }
                        return inst;
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        ASM::Register copy = reg1;
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        inst.push_back(std::make_shared<ASM::Cmp>(reg1, imm));
                        inst.push_back(std::make_shared<ASM::Set>(reg1, ASM::NotEqual));
                        if (reg1.size != ASM::S8) {
                            copy.size = ASM::S8;
                            inst.push_back(std::make_shared<ASM::Move>(reg1, copy, ASM::MoveType::ZeroExtend));
                        }
                        return inst;
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 != longValue2;
                    }
                );
            case OperationType::BinaryLogicalLower:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        ASM::Register copy = reg1;
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        inst.push_back(std::make_shared<ASM::Cmp>(reg1, reg2));
                        inst.push_back(std::make_shared<ASM::Set>(reg1, ASM::Below));
                        if (reg1.size != ASM::S8) {
                            copy.size = ASM::S8;
                            inst.push_back(std::make_shared<ASM::Move>(reg1, copy, ASM::MoveType::ZeroExtend));
                        }
                        return inst;
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        ASM::Register copy = reg1;
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        inst.push_back(std::make_shared<ASM::Cmp>(reg1, imm));
                        inst.push_back(std::make_shared<ASM::Set>(reg1, ASM::Below));
                        if (reg1.size != ASM::S8) {
                            copy.size = ASM::S8;
                            inst.push_back(std::make_shared<ASM::Move>(reg1, copy, ASM::MoveType::ZeroExtend));
                        }
                        return inst;
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 < longValue2;
                    }
                );
            case OperationType::BinaryLogicalLowerEq:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        ASM::Register copy = reg1;
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        inst.push_back(std::make_shared<ASM::Cmp>(reg1, reg2));
                        inst.push_back(std::make_shared<ASM::Set>(reg1, ASM::BelowEqual));
                        if (reg1.size != ASM::S8) {
                            copy.size = ASM::S8;
                            inst.push_back(std::make_shared<ASM::Move>(reg1, copy, ASM::MoveType::ZeroExtend));
                        }
                        return inst;
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        ASM::Register copy = reg1;
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        inst.push_back(std::make_shared<ASM::Cmp>(reg1, imm));
                        inst.push_back(std::make_shared<ASM::Set>(reg1, ASM::BelowEqual));
                        if (reg1.size != ASM::S8) {
                            copy.size = ASM::S8;
                            inst.push_back(std::make_shared<ASM::Move>(reg1, copy, ASM::MoveType::ZeroExtend));
                        }
                        return inst;
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 <= longValue2;
                    }
                );
            case OperationType::BinaryLogicalGreater:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        ASM::Register copy = reg1;
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        inst.push_back(std::make_shared<ASM::Cmp>(reg1, reg2));
                        inst.push_back(std::make_shared<ASM::Set>(reg1, ASM::Above));
                        if (reg1.size != ASM::S8) {
                            copy.size = ASM::S8;
                            inst.push_back(std::make_shared<ASM::Move>(reg1, copy, ASM::MoveType::ZeroExtend));
                        }
                        return inst;
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        ASM::Register copy = reg1;
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        inst.push_back(std::make_shared<ASM::Cmp>(reg1, imm));
                        inst.push_back(std::make_shared<ASM::Set>(reg1, ASM::Above));
                        if (reg1.size != ASM::S8) {
                            copy.size = ASM::S8;
                            inst.push_back(std::make_shared<ASM::Move>(reg1, copy, ASM::MoveType::ZeroExtend));
                        }
                        return inst;
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 > longValue2;
                    }
                );
            case OperationType::BinaryLogicalGreaterEq:
                return generateOperation(
                    op,
                    reg,
                    [this](ASM::Register reg1, ASM::Register reg2) {
                        ASM::Register copy = reg1;
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        inst.push_back(std::make_shared<ASM::Cmp>(reg1, reg2));
                        inst.push_back(std::make_shared<ASM::Set>(reg1, ASM::AboveEqual));
                        if (reg1.size != ASM::S8) {
                            copy.size = ASM::S8;
                            inst.push_back(std::make_shared<ASM::Move>(reg1, copy, ASM::MoveType::ZeroExtend));
                        }
                        return inst;
                    },
                    [this](ASM::Register reg1, const ASM::Immediate &imm) {
                        ASM::Register copy = reg1;
                        std::vector<std::shared_ptr<ASM::Instruction>> inst;
                        inst.push_back(std::make_shared<ASM::Cmp>(reg1, imm));
                        inst.push_back(std::make_shared<ASM::Set>(reg1, ASM::AboveEqual));
                        if (reg1.size != ASM::S8) {
                            copy.size = ASM::S8;
                            inst.push_back(std::make_shared<ASM::Move>(reg1, copy, ASM::MoveType::ZeroExtend));
                        }
                        return inst;
                    },
                    [this](long longValue1, long longValue2) {
                        return longValue1 >= longValue2;
                    }
                );
        }

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::loadExpression(const Expression &exp, ASM::Register reg) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        std::vector<std::shared_ptr<ASM::Instruction>> tmp;
        std::string type = getExpressionType(exp);

        ASM::Register copy = reg;
        reg.size = getRegisterSize(type);

        ASM::Register rax = ASM::RAX;
        rax.size = reg.size;

        Literal lit;
        Index index;

        auto mov = std::make_shared<ASM::Mov>();

        std::wstring_convert<raven::codecvtUTF8, char32_t> conv;

        switch (exp.getVariant()) {
            case ExpressionType::ExpLiteral:
                lit = static_cast<Literal>(exp);
                switch (lit.getVariant()) {
                    case LiteralType::Boolean:
                        mov->setDst(reg).setSrc(ASM::Immediate(static_cast<bool>(lit)));
                        break;
                    case LiteralType::Floating:
                        throw exception::CompilerException("floating value not supported");
                        break;
                    case LiteralType::Character:
                        mov->setDst(reg).setSrc(ASM::Immediate(conv.to_bytes(static_cast<char32_t>(lit)).at(0)));
                        break;
                    case LiteralType::Integral:
                        mov->setDst(reg).setSrc(ASM::Immediate(static_cast<long>(lit)));
                        break;
                    case LiteralType::String:
                        mov->setDst(reg).setSrc(ASM::Immediate(0).setSize(ASM::IMM32));
                        Compiler::addRodata(static_cast<std::string>(lit), [=](size_t offset) {
                            mov->setDst(reg).setSrc(ASM::Immediate(offset).setSize(ASM::IMM32));
                        });
                        break;
                }
                break;
            case ExpressionType::ExpIdentifier:
                return loadVariable(static_cast<Identifier>(exp).getId(), reg);
            case ExpressionType::ExpIndex:
                return loadIndex(static_cast<Index>(exp), reg);
            case ExpressionType::ExpCall:
                tmp = handleCall(static_cast<Call>(exp));
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                if (reg.reg == 0 && !reg.extend)
                    return instructions;
                mov->setDst(reg).setSrc(rax);
                break;
            case ExpressionType::ExpOperation:
                return loadOperation(static_cast<Operation>(exp), reg);
        }

        instructions.push_back(mov);
        functionSize += mov->getSize();

        if (isIntegralType(type) && needPromotion(copy, type)) {
            auto prom = ASM::Move::move(copy, reg);
            instructions.insert(instructions.end(), prom.begin(), prom.end());
            for (auto &it : prom) {
                functionSize += it->getSize();
            }
        }

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::logicExpression(const Expression &exp, ASM::Register reg, bool _not) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        std::vector<std::shared_ptr<ASM::Instruction>> tmp;

        ASM::Register copy = reg;

        tmp = loadExpression(exp, reg);
        instructions.insert(instructions.end(), tmp.begin(), tmp.end());
        instructions.push_back(std::make_shared<ASM::Cmp>(reg, ASM::Immediate(0)));
        functionSize += instructions.back()->getSize();
        instructions.push_back(std::make_shared<ASM::Set>(reg, _not ? ASM::Equal : ASM::NotEqual));
        functionSize += instructions.back()->getSize();
        if (reg.size != ASM::S8) {
            copy.size = ASM::S8;
            instructions.push_back(std::make_shared<ASM::Move>(reg, copy, ASM::MoveType::ZeroExtend));
            functionSize += instructions.back()->getSize();
        }

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::jmpExpression(const Expression &exp, bool &ignoreBase, bool &ignoreAlternate) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        std::vector<std::shared_ptr<ASM::Instruction>> tmp;

        if (exp.getVariant() == ExpressionType::ExpOperation) {
            const Operation &op = static_cast<const Operation&>(exp);
            switch (op.getType()) {
                case OperationType::BinaryLogicalAnd:
                    return generateOperation(
                        op,
                        ASM::RAX,
                        [&](ASM::Register reg1, ASM::Register reg2) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            std::vector<std::shared_ptr<ASM::Instruction>> tmp = logicRegister(reg1, false);
                            inst.insert(inst.end(), tmp.begin(), tmp.end());
                            tmp = logicRegister(reg2, false);
                            inst.insert(inst.end(), tmp.begin(), tmp.end());
                            inst.push_back(std::make_shared<ASM::And>(reg1, reg2));
                            inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::NotEqual));
                            return inst;
                        },
                        [&](ASM::Register reg1, const ASM::Immediate &imm) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            if (imm.getValue()) {
                                inst.push_back(std::make_shared<ASM::And>(reg1, 1));
                                inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::NotEqual));
                                return inst;
                            }
                            ignoreBase = true;
                            inst.push_back(std::make_shared<ASM::Mov>(reg1, 0));
                            return inst;
                        },
                        [&](long longValue1, long longValue2) {
                            if (longValue1 && longValue2)
                                ignoreAlternate = true;
                            else
                                ignoreAlternate = false;
                            return 0L;
                        },
                        true
                    );
                case OperationType::BinaryLogicalOr:
                    return generateOperation(
                        op,
                        ASM::RAX,
                        [&](ASM::Register reg1, ASM::Register reg2) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            std::vector<std::shared_ptr<ASM::Instruction>> tmp = logicRegister(reg1, false);
                            inst.insert(inst.end(), tmp.begin(), tmp.end());
                            tmp = logicRegister(reg2, false);
                            inst.insert(inst.end(), tmp.begin(), tmp.end());
                            inst.push_back(std::make_shared<ASM::Or>(reg1, reg2));
                            inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::NotEqual));
                            return inst;
                        },
                        [&](ASM::Register reg1, const ASM::Immediate &imm) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            if (imm.getValue()) {
                                ignoreAlternate = true;
                                inst.push_back(std::make_shared<ASM::Mov>(reg1, 1));
                            } else {
                                inst.push_back(std::make_shared<ASM::Or>(reg1, 0));
                                inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::NotEqual));
                            }
                            return inst;
                        },
                        [&](long longValue1, long longValue2) {
                            if (longValue1 || longValue2)
                                ignoreAlternate = true;
                            else
                                ignoreAlternate = false;
                            return 0L;
                        },
                        true
                    );
                case OperationType::BinaryLogicalXor:
                    return generateOperation(
                        op,
                        ASM::RAX,
                        [&](ASM::Register reg1, ASM::Register reg2) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            std::vector<std::shared_ptr<ASM::Instruction>> tmp = logicRegister(reg1, false);
                            inst.insert(inst.end(), tmp.begin(), tmp.end());
                            tmp = logicRegister(reg2, false);
                            inst.insert(inst.end(), tmp.begin(), tmp.end());
                            inst.push_back(std::make_shared<ASM::Xor>(reg1, reg2));
                            inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::NotEqual));
                            return inst;
                        },
                        [&](ASM::Register reg1, const ASM::Immediate &imm) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            std::vector<std::shared_ptr<ASM::Instruction>> tmp = logicRegister(reg1, false);
                            inst.insert(inst.end(), tmp.begin(), tmp.end());
                            if (imm.getValue())
                                inst.push_back(std::make_shared<ASM::Xor>(reg1, 1));
                            else
                                inst.push_back(std::make_shared<ASM::Xor>(reg1, 0));
                            inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::NotEqual));
                            return inst;
                        },
                        [&](long longValue1, long longValue2) {
                            if ((!!longValue1) ^ (!!longValue2))
                                ignoreAlternate = true;
                            else
                                ignoreAlternate = false;
                            return 0L;
                        },
                        true
                    );
                case OperationType::BinaryLogicalEqual:
                    return generateOperation(
                        op,
                        ASM::RAX,
                        [&](ASM::Register reg1, ASM::Register reg2) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            inst.push_back(std::make_shared<ASM::Cmp>(reg1, reg2));
                            inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::NotEqual));
                            return inst;
                        },
                        [&](ASM::Register reg1, const ASM::Immediate &imm) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            inst.push_back(std::make_shared<ASM::Cmp>(reg1, imm));
                            inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::NotEqual));
                            return inst;
                        },
                        [&](long longValue1, long longValue2) {
                            if (longValue1 == longValue2)
                                ignoreAlternate = true;
                            else
                                ignoreAlternate = false;
                            return 0L;
                        },
                        true
                    );
                case OperationType::BinaryLogicalNotEqual:
                    return generateOperation(
                        op,
                        ASM::RAX,
                        [&](ASM::Register reg1, ASM::Register reg2) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            inst.push_back(std::make_shared<ASM::Cmp>(reg1, reg2));
                            inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::Equal));
                            return inst;
                        },
                        [&](ASM::Register reg1, const ASM::Immediate &imm) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            inst.push_back(std::make_shared<ASM::Cmp>(reg1, imm));
                            inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::Equal));
                            return inst;
                        },
                        [&](long longValue1, long longValue2) {
                            if (longValue1 != longValue2)
                                ignoreAlternate = true;
                            else
                                ignoreAlternate = false;
                            return 0L;
                        },
                        true
                    );
                case OperationType::BinaryLogicalLower:
                    return generateOperation(
                        op,
                        ASM::RAX,
                        [&](ASM::Register reg1, ASM::Register reg2) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            inst.push_back(std::make_shared<ASM::Cmp>(reg1, reg2));
                            inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::GreaterEqual));
                            return inst;
                        },
                        [&](ASM::Register reg1, const ASM::Immediate &imm) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            inst.push_back(std::make_shared<ASM::Cmp>(reg1, imm));
                            inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::GreaterEqual));
                            return inst;
                        },
                        [&](long longValue1, long longValue2) {
                            if (longValue1 < longValue2)
                                ignoreAlternate = true;
                            else
                                ignoreAlternate = false;
                            return 0L;
                        },
                        true
                    );
                case OperationType::BinaryLogicalLowerEq:
                    return generateOperation(
                        op,
                        ASM::RAX,
                        [&](ASM::Register reg1, ASM::Register reg2) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            inst.push_back(std::make_shared<ASM::Cmp>(reg1, reg2));
                            inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::Greater));
                            return inst;
                        },
                        [&](ASM::Register reg1, const ASM::Immediate &imm) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            inst.push_back(std::make_shared<ASM::Cmp>(reg1, imm));
                            inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::Greater));
                            return inst;
                        },
                        [&](long longValue1, long longValue2) {
                            if (longValue1 <= longValue2)
                                ignoreAlternate = true;
                            else
                                ignoreAlternate = false;
                            return 0L;
                        },
                        true
                    );
                case OperationType::BinaryLogicalGreater:
                    return generateOperation(
                        op,
                        ASM::RAX,
                        [&](ASM::Register reg1, ASM::Register reg2) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            inst.push_back(std::make_shared<ASM::Cmp>(reg1, reg2));
                            inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::LowerEqual));
                            return inst;
                        },
                        [&](ASM::Register reg1, const ASM::Immediate &imm) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            inst.push_back(std::make_shared<ASM::Cmp>(reg1, imm));
                            inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::LowerEqual));
                            return inst;
                        },
                        [&](long longValue1, long longValue2) {
                            if (longValue1 > longValue2)
                                ignoreAlternate = true;
                            else
                                ignoreAlternate = false;
                            return 0L;
                        },
                        true
                    );
                case OperationType::BinaryLogicalGreaterEq:
                    return generateOperation(
                        op,
                        ASM::RAX,
                        [&](ASM::Register reg1, ASM::Register reg2) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            inst.push_back(std::make_shared<ASM::Cmp>(reg1, reg2));
                            inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::Lower));
                            return inst;
                        },
                        [&](ASM::Register reg1, const ASM::Immediate &imm) {
                            std::vector<std::shared_ptr<ASM::Instruction>> inst;
                            inst.push_back(std::make_shared<ASM::Cmp>(reg1, imm));
                            inst.push_back(std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::Lower));
                            return inst;
                        },
                        [&](long longValue1, long longValue2) {
                            if (longValue1 >= longValue2)
                                ignoreAlternate = true;
                            else
                                ignoreAlternate = false;
                            return 0L;
                        },
                        true
                    );
                default:
                    tmp = loadOperation(op, ASM::RAX);
                    instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                    break;

            }
        } else if (exp.getVariant() == ExpressionType::ExpLiteral) {
            Literal lit = static_cast<Literal>(exp);
            switch (lit.getVariant()) {
                case LiteralType::Boolean:
                    if (static_cast<bool>(lit))
                        ignoreAlternate = true;
                    else
                        ignoreBase = true;
                    return instructions;
                case LiteralType::Floating:
                    throw exception::CompilerException("floating value not supported");
                case LiteralType::Character:
                    if (static_cast<char32_t>(lit))
                        ignoreAlternate = true;
                    else
                        ignoreBase = true;
                    return instructions;
                case LiteralType::Integral:
                    if (static_cast<long>(lit))
                        ignoreAlternate = true;
                    else
                        ignoreBase = true;
                    return instructions;
                case LiteralType::String:
                    ignoreAlternate = true;
                    return instructions;
                case LiteralType::Null:
                    ignoreBase = true;
                    return instructions;
            }
        } else {
            tmp = loadExpression(exp, ASM::RAX);
            instructions.insert(instructions.end(), tmp.begin(), tmp.end());
        }

        auto cmp = std::make_shared<ASM::Cmp>(ASM::RAX, ASM::Immediate(0));
        instructions.push_back(cmp);
        functionSize += cmp->getSize();

        auto je = std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32), ASM::Equal);
        instructions.push_back(je);
        functionSize += je->getSize();

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::logicRegister(ASM::Register reg, bool _not) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        std::vector<std::shared_ptr<ASM::Instruction>> tmp;

        ASM::Register copy = reg;

        instructions.push_back(std::make_shared<ASM::Cmp>(reg, ASM::Immediate(0)));
        instructions.push_back(std::make_shared<ASM::Set>(reg, _not ? ASM::Equal : ASM::NotEqual));
        if (reg.size != ASM::S8) {
            copy.size = ASM::S8;
            instructions.push_back(std::make_shared<ASM::Move>(reg, copy, ASM::MoveType::ZeroExtend));
        }

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::handleCall(const Call &call) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;

        std::array<ASM::Register, 6> arguments = {
            ASM::RDI,
            ASM::RSI,
            ASM::RDX,
            ASM::RCX,
            ASM::R8,
            ASM::R9
        };

        size_t ac = call.getArgs().size();
        const FunctionPrototype &prototype = Compiler::getFunction(call.getName());

        if (ac > 6)
            throw exception::CompilerException("Too many arguments in function call");
        if (!callable(call, prototype))
            throw exception::CompilerException("Function '" + call.getName() + "' is not callable with given arguments");

        // Save state

        for (size_t i = 0; i < ac; ++i) {
            auto push = std::make_shared<ASM::Push>();
            push->setValue(arguments[i]);
            instructions.push_back(push);
                        functionSize += push->getSize();
        }

        // Load arguments

        for (size_t i = 0; i < ac; ++i) {
            auto tmp = loadExpression(*call.getArgs().at(i), arguments[i]);
            instructions.insert(instructions.end(), tmp.begin(), tmp.end());
        }

        // Call function

        auto callInstruction = std::make_shared<ASM::Call>();
        Compiler::functionCall(name, call.getName(), functionSize, callInstruction);
        instructions.push_back(callInstruction);
        functionSize += callInstruction->getSize();

        // Restore state

        for (size_t i = 0; i < ac; ++i) {
            auto pop = std::make_shared<ASM::Pop>();
            pop->setRegister(arguments[i]);
            instructions.push_back(pop);
            functionSize += pop->getSize();
        }

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::handleDeclaration(const Declaration &decl, bool preStacked) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        std::vector<std::shared_ptr<ASM::Instruction>> tmp;

        if (decl.getDeclarationType() == DeclarationType::Function)
            throw exception::CompilerException("Cannot declare function in function");

        std::string type = decl.getDeclaration().getType();
        std::string name = decl.getDeclaration().getName();

        if (!preStacked && hasVariable(name))
            throw exception::CompilerException("Variable '" + name + "' already declared");

        const VariableDeclarationBehavior *behavior = dynamic_cast<const VariableDeclarationBehavior*>(&decl.getDeclaration());

        if (behavior->hasDeclarator()) {
            if (!preStacked) {
                instructions.push_back(std::make_shared<ASM::Sub>(ASM::RSP, ASM::Immediate(sizeofType(type))));
                functionSize += instructions.back()->getSize();
            }
            tmp = loadExpression(behavior->getExpression(), ASM::RAX);
            instructions.insert(instructions.end(), tmp.begin(), tmp.end());
            if (preStacked) {
                tmp = createVariable(type, name, ASM::RAX);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
            } else {
                tmp = storeVariable(name, ASM::RAX);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
            }
        } else {
            auto xor_ = std::make_shared<ASM::Xor>();
            xor_->setDst(ASM::RAX).setSrc(ASM::RAX);
            instructions.push_back(xor_);
            functionSize += xor_->getSize();
            tmp = createVariable(type, name, ASM::RAX);
            instructions.insert(instructions.end(), tmp.begin(), tmp.end());
        }

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::handleOperation(const Operation &op) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        std::vector<std::shared_ptr<ASM::Instruction>> tmp;

        switch (op.getType()) {
            case OperationType::UnaryBitwiseNot:
            case OperationType::UnaryArithmeticMinus:
            case OperationType::UnaryLogicalNot:
                // Ignore because doesn't affect the result
                break;
            case OperationType::IncrementPre:
            case OperationType::IncrementPost:
                return loadUpdate(op.getFirstExpression(), ASM::RAX, true);
            case OperationType::DecrementPre:
            case OperationType::DecrementPost:
                return loadUpdate(op.getFirstExpression(), ASM::RAX, false);
            case OperationType::Assignment:
            case OperationType::AddAssignment:
            case OperationType::MinusAssignment:
            case OperationType::MultiplyAssignment:
            case OperationType::DivideAssignment:
            case OperationType::ModuloAssignment:
            case OperationType::PowerAssignment:
            case OperationType::BitwiseAndAssignment:
            case OperationType::BitwiseOrAssignment:
            case OperationType::BitwiseXorAssignment:
            case OperationType::BitwiseRShiftAssignment:
            case OperationType::BitwiseLShiftAssignment:
            case OperationType::BitwiseRRotShiftAssignment:
            case OperationType::BitwiseLRotShiftAssignment:
                return handleAssignement(op);
            default:
                if (op.getFirstExpression().getVariant() == ExpressionType::ExpOperation) {
                    tmp = handleOperation(static_cast<Operation>(op.getFirstExpression()));
                    instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                }
                if (op.getSecondExpression().getVariant() == ExpressionType::ExpOperation) {
                    tmp = handleOperation(static_cast<Operation>(op.getSecondExpression()));
                    instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                }
                // Ignore other expressions type because doesn't affect the result
                break;
        }

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::handleAssignement(const Operation &op) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        std::vector<std::shared_ptr<ASM::Instruction>> tmp;

        ASM::Register d = ASM::RBX;
        ASM::Register s = ASM::R8;

        s.size = getRegisterSize(getExpressionType(op.getFirstExpression()));
        d.size = getRegisterSize(getExpressionType(op.getSecondExpression()));

        tmp = loadExpression(op.getSecondExpression(), d);
        instructions.insert(instructions.end(), tmp.begin(), tmp.end());


        switch (op.getType()) {
            case OperationType::Assignment:
                tmp = storeVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), d);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case OperationType::AddAssignment:
                tmp = loadVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                instructions.push_back(std::make_shared<ASM::Add>(s, d));
                functionSize += instructions.back()->getSize();
                tmp = storeVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case OperationType::MinusAssignment:
                tmp = loadVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                instructions.push_back(std::make_shared<ASM::Sub>(s, d));
                functionSize += instructions.back()->getSize();
                tmp = storeVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case OperationType::MultiplyAssignment:
                tmp = loadVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                tmp = ASM::IMul::mul(s, d);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                for (auto &it : tmp)
                    functionSize += it->getSize();
                tmp = storeVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case OperationType::DivideAssignment:
                tmp = loadVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                tmp = ASM::IDiv::div(s, d);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                for (auto &it : tmp)
                    functionSize += it->getSize();
                tmp = storeVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case OperationType::ModuloAssignment:
                tmp = loadVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                tmp = ASM::IDiv::mod(s, d);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                for (auto &it : tmp)
                    functionSize += it->getSize();
                tmp = storeVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case OperationType::PowerAssignment:
            case OperationType::BitwiseAndAssignment:
                tmp = loadVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                instructions.push_back(std::make_shared<ASM::And>(s, d));
                functionSize += instructions.back()->getSize();
                tmp = storeVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case OperationType::BitwiseOrAssignment:
                tmp = loadVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                instructions.push_back(std::make_shared<ASM::Or>(s, d));
                functionSize += instructions.back()->getSize();
                tmp = storeVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case OperationType::BitwiseXorAssignment:
                tmp = loadVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                instructions.push_back(std::make_shared<ASM::Xor>(s, d));
                functionSize += instructions.back()->getSize();
                tmp = storeVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case OperationType::BitwiseRShiftAssignment:
                tmp = loadVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                tmp = ASM::Shift::sar(s, d);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                for (auto &it : tmp)
                    functionSize += it->getSize();
                tmp = storeVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case OperationType::BitwiseLShiftAssignment:
                tmp = loadVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                tmp = ASM::Shift::sal(s, d);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                for (auto &it : tmp)
                    functionSize += it->getSize();
                tmp = storeVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case OperationType::BitwiseRRotShiftAssignment:
                tmp = loadVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                tmp = ASM::Rotate::ror(s, d);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                for (auto &it : tmp)
                    functionSize += it->getSize();
                tmp = storeVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case OperationType::BitwiseLRotShiftAssignment:
                tmp = loadVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                tmp = ASM::Rotate::rol(s, d);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                for (auto &it : tmp)
                    functionSize += it->getSize();
                tmp = storeVariable(static_cast<Identifier>(op.getFirstExpression()).getId(), s);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            default:
                throw exception::CompilerException("Not an assignement");
        }

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::handleExpression(const Expression &exp) {
        switch (exp.getVariant()) {
            case ExpressionType::ExpLiteral:
            case ExpressionType::ExpIdentifier:
            case ExpressionType::ExpArray:
            case ExpressionType::ExpIndex:
                break;
            case ExpressionType::ExpCall:
                return handleCall(static_cast<Call>(exp));
            case ExpressionType::ExpOperation:
                return handleOperation(static_cast<Operation>(exp));
            case ExpressionType::ExpFold:
                return handleExpression(static_cast<Fold>(exp).getExpression());
        }
        return {};
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::handleIf(const If &if_, const std::string &returnType, bool &doesReturn, bool &alternateReturn) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        std::vector<std::shared_ptr<ASM::Instruction>> tmp;
        size_t blkSize = 0;
        size_t diff = 0;

        bool ifBase = false;
        bool altBase = false;

        ASM::Jmp *jump = nullptr;

        tmp = jmpExpression(if_.getCondition(), ifBase, altBase);
        instructions.insert(instructions.end(), tmp.begin(), tmp.end());

        blkSize = functionSize;

        if (!ifBase && !altBase) {
            jump = dynamic_cast<ASM::Jmp*>(instructions.back().get());
        }

        if (!ifBase) {
            scope = scope / ("main");
            tmp = handleBlock(if_.getBlock(), returnType, doesReturn);
            instructions.insert(instructions.end(), tmp.begin(), tmp.end());
        }

        if (ifBase)
            log::warn << "main branch is never used" << log::endl;
        if (altBase) {
            alternateReturn = doesReturn;
            log::warn << "alternate branch is never used" << log::endl;
        }

        if (if_.hasAlternate() && !altBase) {
            auto jalt = std::make_shared<ASM::Jmp>(ASM::Immediate(0).setSize(ASM::IMM32));
            if (!ifBase) {
                instructions.push_back(jalt);
                functionSize += jalt->getSize();

                blkSize = functionSize - blkSize;
                jump->setValue(ASM::Immediate(blkSize).setSize(ASM::IMM32));

                blkSize = functionSize;
            }


            if (if_.alternateIndex() == 2) {
                bool _1 = false;
                bool _2 = false;
                scope = scope / ("alternate");
                tmp = handleIf(if_.getAlternate<If>(), returnType, _1, _2);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                alternateReturn = _1 && _2;
            } else {
                scope = scope / ("alternate");
                tmp = handleBlock(if_.getAlternate<Block>(), returnType, alternateReturn);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
            }

            if (!ifBase) {
                blkSize = functionSize - blkSize;
                jalt->setValue(ASM::Immediate(blkSize).setSize(ASM::IMM32));
            } else {
                doesReturn = alternateReturn;
            }
        } else {
            if (!ifBase && !altBase) {
                blkSize = functionSize - blkSize;
                jump->setValue(ASM::Immediate(blkSize).setSize(ASM::IMM32));
            }
        }

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::handleWhile(const While &while_, const std::string &returnType, bool &doesReturn) {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        std::vector<std::shared_ptr<ASM::Instruction>> tmp;
        size_t blkSize = 0;
        size_t diff = 0;
        long begin = 0;

        bool ifBase = false;
        bool _ = false;

        ASM::Jmp *jump = nullptr;

        sizeStack.push(offset);
        for (auto &statement : while_.getBlock().getStatements()) {
            std::string name;
            std::string type;
            Declaration decl;
            switch (statement->getVariant()) {
                case StatementType::Declaration:
                    decl = static_cast<Declaration>(*statement.get());
                    name = decl.getDeclaration().getName();
                    type = decl.getDeclaration().getType();
                    if (hasVariable(name))
                        throw exception::CompilerException("Variable '" + name + "' already declared");
                    if (decl.getDeclarationType() == DeclarationType::Function)
                        throw exception::CompilerException("Cannot declare function in function");
                    diff += sizeofType(type);
                    variables[generateKey(scope, name)] = Variable{type, offset, nullptr, false};
                    break;
            }
        }

        if (diff != 0) {
            instructions.push_back(std::make_shared<ASM::Sub>(ASM::RSP, ASM::Immediate(diff)));
            functionSize += instructions.back()->getSize();
        }

        begin = functionSize;

        tmp = jmpExpression(while_.getCondition(), ifBase, _);
        instructions.insert(instructions.end(), tmp.begin(), tmp.end());

        blkSize = functionSize;

        if (!ifBase && !_) {
            jump = dynamic_cast<ASM::Jmp*>(instructions.back().get());
        }

        if (ifBase) {
            log::warn << "while is never used" << log::endl;
            return {};
        }

        scope = scope / ("loop");
        tmp = handleBlock(while_.getBlock(), returnType, doesReturn, true);
        instructions.insert(instructions.end(), tmp.begin(), tmp.end());

        auto jmpB = std::make_shared<ASM::Jmp>(ASM::Immediate(begin - (functionSize + 5)).setSize(ASM::IMM32));
        instructions.push_back(jmpB);
        functionSize += jmpB->getSize();

        if (_ && !doesReturn) {
            log::warn << "while may be infinite" << log::endl;
        }

        if (jump)
            jump->setValue(ASM::Immediate(functionSize - blkSize).setSize(ASM::IMM32));

        if (!doesReturn) {
            scope = scope.parent_path();
            diff = offset - sizeStack.top();
            sizeStack.pop();
            if (diff != 0) {
                auto add = std::make_shared<ASM::Add>(ASM::RSP, ASM::Immediate(diff));
                instructions.push_back(add);
                functionSize += add->getSize();
            }
            offset -= diff;
        }

        return instructions;
    }

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::handleBlock(const Block &block,
                                                                                const std::string &returnType,
                                                                                bool &doesReturn,
                                                                                bool stackOverride)
    {
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        std::vector<std::shared_ptr<ASM::Instruction>> tmp;
        std::shared_ptr<ASM::Instruction> add;

        Return returnInstruction;
        std::string retu;
        long diff = 0;

        bool ifReturn = false;
        bool altReturn = false;

        // Prestack variables

        if (!stackOverride) {
            sizeStack.push(offset);
            for (auto &statement : block.getStatements()) {
                std::string name;
                std::string type;
                Declaration decl;
                switch (statement->getVariant()) {
                    case StatementType::Declaration:
                        decl = static_cast<Declaration>(*statement.get());
                        name = decl.getDeclaration().getName();
                        type = decl.getDeclaration().getType();
                        if (hasVariable(name))
                            throw exception::CompilerException("Variable '" + name + "' already declared");
                        if (decl.getDeclarationType() == DeclarationType::Function)
                            throw exception::CompilerException("Cannot declare function in function");
                        diff += sizeofType(type);
                        variables[generateKey(scope, name)] = Variable{type, offset, nullptr, false};
                        break;
                }
            }

            if (diff != 0) {
                instructions.push_back(std::make_shared<ASM::Sub>(ASM::RSP, ASM::Immediate(diff)));
                functionSize += instructions.back()->getSize();
            }
        }


        for (auto &statement : block.getStatements()) {
            if (doesReturn) {
                log::warn << "Function '" << name << "' have unreachable code" << log::endl;
                break;
            }
            switch (statement->getVariant()) {
                case StatementType::Return:
                    doesReturn = true;
                    returnInstruction = static_cast<Return>(*statement.get());
                    if (returnInstruction.hasExpression()) {
                        retu = getExpressionType(returnInstruction.getExpression());
                        if (!isIntegralType(retu) && !isIntegralType(returnType) && retu != returnType)
                            throw exception::CompilerException("Return type mismatch in function '" + name + "'");
                        tmp = loadExpression(returnInstruction.getExpression(), ASM::RAX);
                        instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                        if (offset != 0) {
                            auto add = std::make_shared<ASM::Add>();
                            add->setDst(ASM::RSP).setSrc(ASM::Immediate(offset));
                            instructions.push_back(add);
                            functionSize += add->getSize();
                        }
                        auto pop = std::make_shared<ASM::Pop>();
                        pop->setRegister(ASM::RBP);
                        instructions.push_back(pop);
                        auto ret = std::make_shared<ASM::Ret>();
                        instructions.push_back(ret);
                        functionSize += pop->getSize();
                        functionSize += ret->getSize();
                    } else {
                        throw exception::CompilerException("Cannot return void in function '" + name + "'");
                    }
                    break;
                case StatementType::Expression:
                    tmp = handleExpression(static_cast<Expression>(*statement.get()));
                    instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                    break;
                case StatementType::Declaration:
                    tmp = handleDeclaration(static_cast<Declaration>(*statement.get()), true);
                    instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                    break;
                case StatementType::Block:
                    scope = scope / ("block");
                    tmp = handleBlock(static_cast<Block>(*statement.get()), returnType, doesReturn);
                    instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                    break;
                case StatementType::If:
                    tmp = handleIf(static_cast<If>(*statement.get()), returnType, ifReturn, altReturn);
                    instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                    if (ifReturn && altReturn)
                        doesReturn = true;
                case StatementType::Loop:
                    tmp = handleWhile(static_cast<While>(*statement.get()), returnType, doesReturn);
                    instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                    break;
            }
        }

        if (!stackOverride) {
            if (scope != "root") {
                scope = scope.parent_path();
                diff = offset - sizeStack.top();
                sizeStack.pop();
                if (diff != 0) {
                    add = std::make_shared<ASM::Add>(ASM::RSP, ASM::Immediate(diff));
                    instructions.push_back(add);
                    functionSize += add->getSize();
                }
                offset -= diff;
            }
        }

        return instructions;
    }

    bool FunctionCompiler::callable(const Call &call, const FunctionPrototype &prototype) {
        if (call.getArgs().size() != prototype.getArgs().size())
            return false;
        for (size_t i = 0; i < call.getArgs().size(); i++) {
            if (getExpressionType(*call.getArgs().at(i)) != prototype.getArgs().at(i))
                return false;
        }
        return true;
    }

    FunctionCompiler::FunctionCompiler(const std::string &name, const std::string &ret) : name(name), ret(ret) {}

    std::vector<std::shared_ptr<ASM::Instruction>> FunctionCompiler::compile(const Declaration &func) {
        const FunctionDeclarationBehavior *behavior = dynamic_cast<const FunctionDeclarationBehavior*>(&func.getDeclaration());
        if (behavior == nullptr)
            return {};
        std::vector<std::shared_ptr<ASM::Instruction>> instructions;
        std::vector<std::shared_ptr<ASM::Instruction>> tmp;

        std::array<ASM::Register, 6> arguments = {
            ASM::RDI,
            ASM::RSI,
            ASM::RDX,
            ASM::RCX,
            ASM::R8,
            ASM::R9
        };

        auto push = std::make_shared<ASM::Push>();
        push->setValue(ASM::RBP);
        instructions.push_back(push);
        auto mov = std::make_shared<ASM::Mov>();
        mov->setDst(ASM::RBP).setSrc(ASM::RSP);
        instructions.push_back(mov);

        functionSize += push->getSize();
        functionSize += mov->getSize();

        int idx = 0;
        for (auto &[type, name] : behavior->getArgs()) {
            auto tmp2 = createVariable(type, name, arguments[idx]);
            ++idx;
            tmp.insert(tmp.end(), tmp2.begin(), tmp2.end());
        }

        if (idx != 0) {
            auto sub = std::make_shared<ASM::Sub>();
            sub->setDst(ASM::RSP).setSrc(ASM::Immediate(offset));
            instructions.push_back(sub);
            instructions.insert(instructions.end(), tmp.begin(), tmp.end());
            functionSize += sub->getSize();
        }

        bool doesReturn = false;

        tmp = handleBlock(behavior->getBlock(), behavior->getType(), doesReturn);
        instructions.insert(instructions.end(), tmp.begin(), tmp.end());

        if (!doesReturn) {
            log::warn << "Function '" << name << "' does not return" << log::endl;

            mov = std::make_shared<ASM::Mov>();
            mov->setDst(ASM::RAX).setSrc(ASM::Immediate(0));
            instructions.push_back(mov);
            functionSize += mov->getSize();

            if (offset != 0) {
                auto add = std::make_shared<ASM::Add>();
                add->setDst(ASM::RSP).setSrc(ASM::Immediate(offset));
                instructions.push_back(add);
                functionSize += add->getSize();
            }
            auto pop = std::make_shared<ASM::Pop>();
            pop->setRegister(ASM::RBP);
            instructions.push_back(pop);
            auto ret = std::make_shared<ASM::Ret>();
            instructions.push_back(ret);
            functionSize += pop->getSize();
            functionSize += ret->getSize();
        }

        return instructions;
    }

    std::vector<uint8_t> Compiler::__start() {
        static std::vector<uint8_t> code = {
            0xf3, 0x0f, 0x1e, 0xfa,                         // endbr64
            0x31, 0xed,                                     // xor ebp, ebp
            0x5f,                                           // pop rdi
            0x48, 0x89, 0xe6,                               // mov rsi, rsp
            0x48, 0x83, 0xe4, 0xf0,                         // and rsp, 0xfffffffffffffff0
            0x50,                                           // push rax
            0x54,                                           // push rsp
            0xE8, 0x0c, 0x00, 0x00, 0x00,                   // call main
            0x48, 0x89, 0xC7,                               // mov rdi, rax
            0x48, 0xC7, 0xC0, 0x3C, 0x00, 0x00, 0x00,       // mov rax, 60
            0x0F, 0x05                                      // syscall
        };
        return code;
    }

    std::vector<Declaration> Compiler::parse(std::vector<std::string> &files) {
        std::vector<Declaration> decls;
        bool main = false;
        for (auto &file : files) {
            uint8_t *data = reinterpret_cast<uint8_t *>(ravenSerialize(reinterpret_cast<void*>(file.data())));
            if (data == nullptr)
                throw exception::CompilerException("Cannot open file '" + file + "'");
            Input input(data);
            while (*input.getData() != 255) {
                try {
                    decls.push_back(unserialize<Declaration>(input));
                    if (decls.back().getDeclaration().getName() == "main") {
                        if (main)
                            throw exception::CompilerException("Multiple main functions found");
                        main = true;
                    }
                } catch (exception::SerializationException &e) {
                    log::error << e.what() << log::endl;
                    throw exception::CompilerException("Cannot parse file '" + file + "'");
                }
            }
        }
        if (!main)
            throw exception::CompilerException("No main functions found");
        return decls;
    }

    void Compiler::setup() {
        auto strtab = std::make_shared<elf::StrSection>();
        auto symtab = std::make_shared<elf::Symtab>();
        auto stdlib = std::make_shared<elf::Text>();
        auto shstrtab = std::make_shared<elf::StrSection>();
        auto text = std::make_shared<elf::Text>();
        auto rodata = std::make_shared<elf::Section>();
        auto data = std::make_shared<elf::Section>();
        auto bss = std::make_shared<elf::Section>();

        _sections["symtab"] = symtab;
        _sections["strtab"] = strtab;
        _sections["shstrtab"] = shstrtab;
        _sections["text"] = text;
        _sections["stdlib"] = stdlib;
        _sections["rodata"] = rodata;
        _sections["data"] = data;
        _sections["bss"] = bss;

        std::vector<uint8_t> libCode = stdlib::getStandardLibrary();
        stdlib->pushData(libCode);

        _sectionHeaders[".symtab"] = elf::SectionHeader().setName(".symtab");
        _sectionHeaders[".strtab"] = elf::SectionHeader().setName(".strtab");
        _sectionHeaders[".shstrtab"] = elf::SectionHeader().setName(".shstrtab");
        _sectionHeaders[".text"] = elf::SectionHeader().setName(".text");
        _sectionHeaders[".stdlib"] = elf::SectionHeader().setName(".stdlib");
        _sectionHeaders[".rodata"] = elf::SectionHeader().setName(".rodata");
        _sectionHeaders[".data"] = elf::SectionHeader().setName(".data").setType(elf::SectionHeaderType::SProgbits);
        _sectionHeaders[".bss"] = elf::SectionHeader().setName(".bss").setType(elf::SectionHeaderType::SNoBits);

        _sectionHeaders[".symtab"].bindSection(symtab.get());
        _sectionHeaders[".strtab"].bindSection(strtab.get());
        _sectionHeaders[".shstrtab"].bindSection(shstrtab.get());
        _sectionHeaders[".text"].bindSection(text.get());
        _sectionHeaders[".stdlib"].bindSection(stdlib.get());
        _sectionHeaders[".rodata"].bindSection(rodata.get());
        _sectionHeaders[".data"].bindSection(data.get());
        _sectionHeaders[".bss"].bindSection(bss.get());

    }

    Declaration Compiler::loadSymbols(const std::vector<Declaration> &decls) {
        std::vector<elf::Symbol> symbols;
        size_t dataOffset = 0;
        size_t bssOffset = 0;

        Declaration main;

        for (auto &d : decls) {
            elf::Symbol sym;
            switch (d.getDeclarationType()) {
                case DeclarationType::Function:
                    for (auto &[key, _] : _symbols) {
                        if (key == d.getDeclaration().getName())
                            throw exception::CompilerException("Function " + d.getDeclaration().getName() + " already defined");
                    }
                    _functions[d.getDeclaration().getName()] = FunctionPrototype(dynamic_cast<const FunctionDeclarationBehavior&>(d.getDeclaration()));
                    sym.setName(d.getDeclaration().getName())
                        .setOther(elf::Default)
                        .setSize(0)
                        .setValue(0x400000);
                    if (d.getDeclaration().getName() == "main") {
                        sym.setInfo(elf::BGlobal, elf::TFunc);
                        main = d;
                    } else
                        sym.setInfo(elf::BLocal, elf::TFunc);
                    symbols.push_back(sym);
                    break;
                case DeclarationType::Variable:
                    for (auto &[key, _] : _symbols) {
                        if (key == d.getDeclaration().getName())
                            throw exception::CompilerException("Symbol " + d.getDeclaration().getName() + " already defined");
                    }
                    bool hasValue = dynamic_cast<const VariableDeclarationBehavior&>(d.getDeclaration()).hasDeclarator();
                    sym.setName(d.getDeclaration().getName())
                        .setOther(elf::Default)
                        .setSize(sizeofType(d.getDeclaration().getType()))
                        .setValue(0x400000)
                        .setInfo(elf::BLocal, elf::TObject);
                    if (hasValue) {
                        _data[d.getDeclaration().getName()] = Variable(d.getDeclaration().getType(), dataOffset, std::make_shared<Expression>(dynamic_cast<const VariableDeclarationBehavior&>(d.getDeclaration()).getExpression()), true);
                        dataOffset += sizeofType(d.getDeclaration().getType());
                    } else {
                        _bss[d.getDeclaration().getName()] = Variable(d.getDeclaration().getType(), bssOffset, nullptr, true);
                        bssOffset += sizeofType(d.getDeclaration().getType());
                    }
                    symbols.push_back(sym);
                    break;
            }
        }

        for (auto &sym : symbols) {
            getSection<elf::StrSection>("strtab")->streamSymbol(sym);
            _symbols[sym.getName()] = sym;
        }

        return main;
    }

    int Compiler::compile(int argc, char *argv[]) {
        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " <source file>" << std::endl;
            return 1;
        }

        std::initializer_list<arg::Constraint> constraints = {
            {"o", {"output"}, "Output file", true, true, "a.out", "file"},
            {"v", {"verbose"}, "Verbose mode", true, false},
            {"h", {"help"}, "Display this help message", true, false}
        };

        arg::Parser parser(argv, constraints);
        if (parser.bad()) {
            parser.help();
            return 1;
        }

        if (parser.has({"h", "help"})) {
            parser.help();
            return 0;
        }

        if (parser.has({"o", "output"}))
            _option.output = parser.query({"o", "output"});
        _option.verbose = parser.has({"v", "verbose"});

        hs_init(&argc, &argv);
        try {
            std::vector<Declaration> decls = parse(parser.args());

            setup();

            for (auto &prot : stdlib::getStandardLibraryDeclarations()) {
                if (_option.verbose)
                    log::info << "Loading function '" << prot.getName() << "'" << log::endl;
                _functions[prot.getName()] = prot;
            }

            Declaration mainDecl = loadSymbols(decls);

            elf::StrSection *strtab = getSection<elf::StrSection>("strtab");
            elf::Text *text = getSection<elf::Text>("text");

            text->pushData(__start());

            _symbols["__start"] = elf::Symbol().setName("__start")
                .setInfo(elf::BGlobal)
                .setOther(elf::Default)
                .setSize(0)
                .setValue(0x400000);

            strtab->streamSymbol(_symbols["__start"]);

            size_t s = 0;

            // Loading main function
            elf::Symbol &main = _symbols["main"];
            main.setValue(main.getValue() + text->getSize());
            FunctionCompiler mainCompiler("main", mainDecl.getDeclaration().getType());
            for (auto &ptr : mainCompiler.compile(mainDecl)) {
                s += ptr->getSize();
                text->addInstruction(ptr);
            }

            for (auto &decl : decls) {
                if (decl.getDeclaration().getName() == "main")
                    continue;
                elf::Symbol &sym = _symbols[decl.getDeclaration().getName()];
                sym.setValue(sym.getValue() + text->getSize());
                FunctionCompiler compiler(decl.getDeclaration().getName(), decl.getDeclaration().getType());
                for (auto &ptr : compiler.compile(decl)) {
                    s += ptr->getSize();
                    text->addInstruction(ptr);
                }
            }

            std::fstream exec;
            exec.open(_option.output, std::ios::out | std::ios::binary | std::fstream::out | std::fstream::trunc);
            std::filesystem::permissions(_option.output, std::filesystem::perms::owner_all);

            generateElf().dump(exec);
        } catch (exception::CompilerException &e) {
            log::error << e.what() << log::endl;
            return 1;
        }
        // catch (exception::ASMException &e) {
        //     log::error << e.what() << log::endl;
        //     return 1;
        // }

        hs_exit();

        return 0;
    }

    const CompilerOption &Compiler::getOption() {
        return _option;
    }

    const FunctionPrototype &Compiler::getFunction(const std::string &name) {
        if (_functions.find(name) == _functions.end())
            throw exception::CompilerException("Function '" + name + "' not found");
        return _functions.at(name);
    }

    void Compiler::functionCall(const std::string &caller, const std::string &callee, size_t offset, std::shared_ptr<ASM::Call> instruction) {
        _calls.push_back(std::make_tuple(caller, callee, offset, instruction));
    }

    void Compiler::addRodata(const std::string &value, const std::function<void(size_t)> &callback) {
        if (_rodataCallbacks.find(value) == _rodataCallbacks.end())
            _rodataCallbacks[value] = {};
        _rodataCallbacks[value].push_back(callback);
    }

    void Compiler::addDependency(const Dependency &dependency) {
        _dependencies.push_back(dependency);
    }

    Dependency &Compiler::getDependenciesFromCaller(const std::string &caller) {
        for (auto &dep : _dependencies) {
            if (dep.caller == caller)
                return dep;
        }
        throw exception::CompilerException("No dependency found for caller '" + caller + "'");
    }

    std::vector<Dependency*> Compiler::getDependenciesFromCallee(const std::string &callee) {
        std::vector<Dependency*> res;
        for (auto &dep : _dependencies) {
            if (dep.callee == callee)
                res.push_back(&dep);
        }
        return res;
    }

    Variable &Compiler::getVariable(const std::string &name) {
        if (_data.find(name) != _data.end())
            return _data.at(name);
        if (_bss.find(name) != _bss.end())
            return _bss.at(name);
        throw exception::CompilerException("Variable '" + name + "' not found");
    }

    bool Compiler::hasVariable(const std::string &name) {
        return _data.find(name) != _data.end() || _bss.find(name) != _bss.end();
    }

    void Compiler::addVariable(const std::string &name, const std::function<void(size_t)> &callback) {
        if (_data.find(name) != _data.end())
            _dataCallbacks[name].push_back(callback);
        else if (_bss.find(name) != _bss.end())
            _bssCallbacks[name].push_back(callback);
        else
            throw exception::CompilerException("Variable '" + name + "' not found");
    }

    elf::Elf Compiler::generateElf() {
        elf::Elf elf;
        size_t offset = 0x40;

        std::vector<uint8_t> dump;

        for (auto &[n, var] : _data)
            dumpExpression(n, var.type, *var.value, dump);

        bool gotText = getSection<elf::Text>("text")->getSize() != 0;
        bool gotRodata = !_rodataCallbacks.empty();
        bool gotData = !_data.empty();
        bool gotBss = !_bss.empty();

        for (auto &d : _dependencies)
            if (d.type == RodataDependency) {
                gotRodata = true;
                break;
            }

        size_t size = 0x38 * (_headers.size() + 2 + gotText + gotRodata + gotData + gotBss);

        elf::ProgramHeader global;
        elf::ProgramHeader plib;
        elf::ProgramHeader ptext;
        elf::ProgramHeader prodata;
        elf::ProgramHeader pdata;
        elf::ProgramHeader pbss;

        plib.setType(elf::ProgramHeaderType::PLoad)
            .setAlign(0x1000)
            .setFlags(elf::ProgramHeaderFlag::Readable | elf::ProgramHeaderFlag::Executable);

        ptext.setType(elf::ProgramHeaderType::PLoad)
            .setAlign(0x1000)
            .setFlags(elf::ProgramHeaderFlag::Readable | elf::ProgramHeaderFlag::Executable);

        prodata.setType(elf::ProgramHeaderType::PLoad)
            .setAlign(0x1000)
            .setFlags(elf::ProgramHeaderFlag::Readable);

        pdata.setType(elf::ProgramHeaderType::PLoad)
            .setAlign(0x1000)
            .setFlags(elf::ProgramHeaderFlag::Readable | elf::ProgramHeaderFlag::Writable);

        pbss.setType(elf::ProgramHeaderType::PLoad)
            .setAlign(0x1000)
            .setFlags(elf::ProgramHeaderFlag::Readable | elf::ProgramHeaderFlag::Writable);

        global.setType(elf::ProgramHeaderType::PLoad)
            .setOffset(0x0)
            .setVaddr(0x400000)
            .setPaddr(0x400000)
            .setAlign(0x1000)
            .setFilesz(0x40 + size)
            .setMemsz(0x40 + size)
            .setFlags(elf::ProgramHeaderFlag::Readable);

        elf::Symtab *symtab = getSection<elf::Symtab>("symtab");
        elf::StrSection *strtab = getSection<elf::StrSection>("strtab");
        elf::StrSection *shstrtab = getSection<elf::StrSection>("shstrtab");
        elf::Text *section = getSection<elf::Text>("stdlib");

        elf.addProgramHeader(global);

        offset += size;
        offset += (0x10 - (offset & 0xf)) & 0xf; // align to 16

        if (_option.verbose)
            log::debug << "Offset: 0x" << std::hex << offset << log::endl;

        uint16_t shndx = 1;

        // Bss section

        if (gotBss) {
            if (_option.verbose)
                log::info << "Loading bss section" << log::endl;
            elf::Section *data = getSection<elf::Section>("bss");
            std::vector<uint8_t> d;
            size_t hand = offset;

            for (auto &[n, var] : _bss) {
                if (_option.verbose)
                    log::info << "Generating " << sizeofType(var.type) << " bytes for " << n << log::endl;
                for (size_t i = 0; i < sizeofType(var.type); i++)
                    d.push_back(0);
                _symbols[n].setShndx(shndx);
                _symbols[n].setValue(0x400000 + hand);
                hand += sizeofType(var.type);
            }
            hand = offset;

            for (auto &[name, callbacks] : _bssCallbacks) {
                for (auto &callback : callbacks) {
                    if (_option.verbose)
                        log::info << "Setting " << name << " offset to 0x" << std::hex << hand << log::endl;
                    callback(hand);
                }
                hand += sizeofType(_bss[name].type);
            }
            ++shndx;

            data->pushData(d);

            pbss.setOffset(offset)
                .setVaddr(0x400000 + offset)
                .setPaddr(0x400000 + offset)
                .setFilesz(data->getSize())
                .setMemsz(data->getSize());

            _sectionHeaders[".bss"].setOffset(offset)
                .setAddr(0x400000 + offset)
                .setSize(data->getSize());

            elf.addProgramHeader(pbss);

            offset += data->getSize();
            offset += (0x10 - (offset & 0xf)) & 0xf;

            if (_option.verbose)
                log::debug << "Offset: 0x" << std::hex << offset << log::endl;
        } else {
            if (_option.verbose)
                log::info << "No bss section found, removing" << log::endl;
            _sectionHeaders.erase(".bss");
            _sections.erase("bss");
        }

        // Data section

        if (gotData) {
            if (_option.verbose)
                log::info << "Loading data section" << log::endl;
            elf::Section *data = getSection<elf::Section>("data");
            std::vector<uint8_t> d;

            for (auto &[n, var] : _data) {
                if (_option.verbose)
                    log::info << "Dumping " << sizeofType(var.type) << " bytes for " << n << log::endl;

                _symbols[n].setShndx(shndx);
                _symbols[n].setValue(0x400000 + offset + var.offset);
            }

            for (size_t i = 0; i < dump.size(); ++i)
                d.push_back(0);

            for (auto &[name, callbacks] : _dataCallbacks) {
                for (auto &callback : callbacks) {
                    if (_option.verbose)
                        log::info << "Setting " << name << " offset to 0x" << std::hex << (0x400000 + offset + _data[name].offset) << log::endl;
                    callback(0x400000 + offset + _data[name].offset);
                }
            }
            ++shndx;

            size_t off = 0;
            for (auto &[n, var] : _data) {
                if (_option.verbose)
                    log::info << "Inserting " << sizeofType(var.type) << " bytes for " << n << " at 0x" << var.offset << log::endl;
                std::memcpy(&d[var.offset], dump.data() + off, sizeofType(var.type));
                off += sizeofType(var.type);
            }

            data->pushData(d);

            pdata.setOffset(offset)
                .setVaddr(0x400000 + offset)
                .setPaddr(0x400000 + offset)
                .setFilesz(data->getSize())
                .setMemsz(data->getSize());

            _sectionHeaders[".data"].setOffset(offset)
                .setAddr(0x400000 + offset)
                .setSize(data->getSize());

            elf.addProgramHeader(pdata);

            offset += data->getSize();
            offset += (0x10 - (offset & 0xf)) & 0xf;

            if (_option.verbose)
                log::debug << "Offset: 0x" << std::hex << offset << log::endl;
        } else {
            if (_option.verbose)
                log::info << "No data section found, removing" << log::endl;
            _sectionHeaders.erase(".data");
            _sections.erase("data");
        }

        // Rodata section

        std::unordered_map<std::string, size_t> rodataOffsets; // to resolve dependencies

        if (gotRodata) {
            if (_option.verbose)
                log::info << "Loading rodata section" << log::endl;
            elf::Section *rodata = getSection<elf::Section>("rodata");
            std::vector<uint8_t> data;
            size_t hand = offset;

            for (auto &[str, callbacks] : _rodataCallbacks) {
                for (auto &callback : callbacks) {
                    callback(0x400000 + hand);
                }
                if (_option.verbose)
                    log::info << "Inserting '" << str << "'" << " at 0x" << std::hex << hand << log::endl;
                data.insert(data.end(), str.begin(), str.end());
                data.push_back(0);
                rodataOffsets[str] = 0x400000 + hand;
                hand += str.size() + 1;
            }
            for (auto &dep : _dependencies) {
                if (dep.type == RodataDependency) {
                    if (_option.verbose)
                        log::info << "Inserting '" << dep.callee << "'" << " at 0x" << std::hex << hand << log::endl;
                    data.insert(data.end(), dep.callee.begin(), dep.callee.end());
                    data.push_back(0);
                    rodataOffsets[dep.callee] = 0x400000 + hand;
                    hand += dep.callee.size() + 1;
                }
            }

            rodata->pushData(data);

            prodata.setOffset(offset)
                .setVaddr(0x400000 + offset)
                .setPaddr(0x400000 + offset)
                .setFilesz(rodata->getSize())
                .setMemsz(rodata->getSize());

            _sectionHeaders[".rodata"].setOffset(offset)
                .setAddr(0x400000 + offset)
                .setSize(rodata->getSize());

            elf.addProgramHeader(prodata);

            offset += rodata->getSize();
            offset += (0x10 - (offset & 0xf)) & 0xf;

            if (_option.verbose)
                log::debug << "Offset: 0x" << std::hex << offset << log::endl;
            ++shndx;
        } else {
            if (_option.verbose)
                log::info << "No rodata section found, removing" << log::endl;
            _sectionHeaders.erase(".rodata");
            _sections.erase("rodata");
        }

        // Standard library

        if (_option.verbose)
            log::info << "Loading stdlib section" << log::endl;

        for (auto &sym : stdlib::getStandardLibrarySymbols()) {
            strtab->streamSymbol(sym);
            sym.setValue(sym.getValue() + offset);
            sym.setShndx(shndx);
            if (sym.getName().starts_with("__"))
                symtab->pushSymbol(sym);
            else
                _symbols[sym.getName()] = sym;
        }
        ++shndx;

        plib.setOffset(offset)
            .setVaddr(0x400000 + offset)
            .setPaddr(0x400000 + offset)
            .setFilesz(section->getSize())
            .setMemsz(section->getSize());

        elf.addProgramHeader(plib);

        _sectionHeaders[".stdlib"].setOffset(offset)
            .setAddr(0x400000 + offset);

        offset += section->getSize();
        offset += (0x10 - (offset & 0xf)) & 0xf;

        if (_option.verbose)
            log::debug << "Offset: 0x" << std::hex << offset << log::endl;

        // Text section

        if (gotText) {
            if (_option.verbose)
                log::info << "Loading text section" << log::endl;
            section = getSection<elf::Text>("text");

            for (auto &[n, sym] : _symbols) {
                if (sym.getShndx() == 0) {
                    if (_option.verbose)
                        log::info << "Loading " << n << " at 0x" << std::hex << (sym.getValue() + offset) << log::endl;
                    sym.setValue(sym.getValue() + offset);
                    sym.setShndx(shndx);
                }
                if (_option.verbose) {
                    log::debug << "Pushing " << n << " into the table" << log::endl;
                    log::debug << "Symbol Shndx: " << sym.getShndx() << log::endl;
                    log::debug << "Symbol Value: " << std::hex << sym.getValue() << log::endl;
                }
                symtab->pushSymbol(sym);
            }
            ++shndx;

            ptext.setOffset(offset)
                .setVaddr(0x400000 + offset)
                .setPaddr(0x400000 + offset)
                .setFilesz(section->getSize())
                .setMemsz(section->getSize());

            _sectionHeaders[".text"].setOffset(offset)
                .setAddr(0x400000 + offset)
                .setSize(section->getSize());

            elf.addProgramHeader(ptext);

            offset += section->getSize();
            offset += (0x10 - (offset & 0xf)) & 0xf;

            if (_option.verbose)
                log::debug << "Offset: 0x" << std::hex << offset << log::endl;
        } else {
            if (_option.verbose)
                log::info << "No text section found, removing" << log::endl;
            _sectionHeaders.erase(".text");
            _sections.erase("text");
        }

        // Resolve calls

        for (auto &[caller, callee, offset, instruction] : _calls) {
            if (_option.verbose)
                log::info << "Resolving call '" << callee << "' inside " << caller << log::endl;
            int64_t callerOff;
            int64_t calleeOff;
            if (_symbols.find(callee) == _symbols.end())
                throw exception::CompilerException("Function '" + callee + "' not found");
            calleeOff = _symbols[callee].getValue();
            if (_symbols.find(caller) == _symbols.end())
                throw exception::CompilerException("Function '" + caller + "' not found");
            callerOff = _symbols[caller].getValue();
            if (_option.verbose) {
                log::info << "Callee: 0x" << std::hex << calleeOff << log::endl;
                log::info << "Caller: 0x" << std::hex << callerOff << log::endl;
                log::info << "Calling point: 0x" << std::hex << (callerOff + offset) << log::endl;
            }
            int64_t off = calleeOff - (callerOff + offset + 5);
            instruction->setValue(ASM::Immediate(off).setSize(ASM::IMM32));
            if (_option.verbose)
                log::info << "Resolving call '" << callee << "' inside " << caller << " with offset 0x" << std::hex << off << log::endl;
        }

        // Resolving dependencies

        if (!_dependencies.empty()) {
            elf::Section *data = getSection<elf::Section>("data");

            for (auto &dep : _dependencies) {
                if (_option.verbose)
                    log::info << "Resolving dependency for '" << dep.caller << "'" << log::endl;
                if (_data.find(dep.caller) == _data.end())
                    throw exception::CompilerException("Variable '" + dep.caller + "' not found");
                switch (dep.type) {
                    case RodataDependency:
                        if (rodataOffsets.find(dep.callee) == rodataOffsets.end())
                            throw exception::CompilerException("Rodata '" + dep.callee + "' not found");
                        std::memcpy(data->getData().data() + _data[dep.caller].offset, &rodataOffsets[dep.callee], sizeof(uint64_t));
                        break;
                    case VariableDependency:
                        if (_data.find(dep.callee) == _data.end()) {
                            if (_bss.find(dep.callee) == _bss.end())
                                throw exception::CompilerException("Variable '" + dep.callee + "' not found");
                            else
                                std::memset(data->getData().data() + _data[dep.caller].offset, 0, sizeofType(_data[dep.caller].type));
                        } else {
                            try {
                                Dependency &dep2 = getDependenciesFromCaller(dep.callee);
                                switch (dep2.type) {
                                    case VariableDependency:
                                        throw exception::CompilerException("Could not resolve dependency for '" + dep.caller + "', variable '" + dep.callee + "' is dependent of '" + dep2.callee + "'");
                                    case RodataDependency:
                                        std::memcpy(data->getData().data() + _data[dep.caller].offset, &rodataOffsets[dep2.callee], sizeof(uint64_t));
                                        break;
                                    case FunctionDependency:
                                        throw exception::CompilerException("Not implemented");
                                }
                            } catch (exception::CompilerException &e) {
                                if (_option.verbose)
                                    log::info << e.what() << log::endl;
                                std::memcpy(data->getData().data() + _data[dep.caller].offset, data->getData().data() + _data[dep.callee].offset, sizeofType(_data[dep.caller].type));
                            }
                        }
                        break;
                    case FunctionDependency:
                        throw exception::CompilerException("Not implemented");
                }
            }
        }

        // Finalize

        for (auto &[_, section] : _sectionHeaders)
            shstrtab->streamSectionHeader(section);

        for (auto &header : _headers)
            elf.addProgramHeader(header);

        if (gotBss)
            elf.addSectionHeader(_sectionHeaders[".bss"]);
        if (gotData)
            elf.addSectionHeader(_sectionHeaders[".data"]);
        if (gotRodata)
            elf.addSectionHeader(_sectionHeaders[".rodata"]);
        elf.addSectionHeader(_sectionHeaders[".stdlib"]);
        if (gotText)
            elf.addSectionHeader(_sectionHeaders[".text"]);


        for (auto &[n, section] : _sectionHeaders) {
            if (n == ".text" || n == ".stdlib" || n == ".rodata" || n == ".data" || n == ".bss")
                continue;
            section.setOffset(offset)
                .setAddr(0x400000 + offset);
            elf.addSectionHeader(section);
            offset += section.getSize();
            offset += (0x10 - (offset & 0xf)) & 0xf;
        }

        elf.setEntry(_symbols["__start"].getValue())
            .setSectionHeaderOffset(offset);
        return elf;
    }

    std::unordered_map<std::string, elf::Symbol> Compiler::_symbols;
    std::unordered_map<std::string, FunctionPrototype> Compiler::_functions;
    std::unordered_map<std::string, elf::SectionHeader> Compiler::_sectionHeaders;
    std::unordered_map<std::string, std::shared_ptr<elf::Section>> Compiler::_sections;
    std::vector<
        std::tuple<
            std::string, // caller
            std::string, // callee
            size_t, // caller offset
            std::shared_ptr<ASM::Call> // instruction
        >
    > Compiler::_calls;
    std::vector<elf::ProgramHeader> Compiler::_headers;

    std::unordered_map<std::string, std::vector<std::function<void(size_t)>>> Compiler::_rodataCallbacks;
    std::unordered_map<std::string, Variable> Compiler::_data;
    std::unordered_map<std::string, std::vector<std::function<void(size_t)>>> Compiler::_dataCallbacks;
    std::unordered_map<std::string, Variable> Compiler::_bss;
    std::unordered_map<std::string, std::vector<std::function<void(size_t)>>> Compiler::_bssCallbacks;

    std::vector<Dependency> Compiler::_dependencies;

    CompilerOption Compiler::_option;

}
