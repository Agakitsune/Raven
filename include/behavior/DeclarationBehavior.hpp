/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** DeclarationBehavior
*/

#pragma once

#include <string>
#include <optional>
#include <vector>
#include <tuple>

#include "Input.hpp"
#include "Expression.hpp"
#include "Block.hpp"

namespace raven {

    class DeclarationBehavior {
        public:
            DeclarationBehavior() = default;
            virtual ~DeclarationBehavior() = default;

            virtual void unserializeDecl(Input &input) = 0;
            virtual const std::string &getType() const = 0;
            virtual const std::string &getName() const = 0;

    };

    class VariableDeclarationBehavior : public DeclarationBehavior {
        std::string _type;
        std::string _name;
        std::optional<Expression> _declarator;

        public:
            VariableDeclarationBehavior() = default;
            VariableDeclarationBehavior(const VariableDeclarationBehavior &other) = default;
            VariableDeclarationBehavior(VariableDeclarationBehavior &&other) = default;
            virtual ~VariableDeclarationBehavior() override = default;

            virtual void unserializeDecl(Input &input) override;

            virtual const std::string &getType() const override;
            virtual const std::string &getName() const override;

            bool hasDeclarator() const;
            const Expression &getExpression() const;

    };

    class FunctionDeclarationBehavior : public DeclarationBehavior {
        std::string _type;
        std::string _name;
        std::vector<std::tuple<std::string, std::string>> _args;
        Block _blk;

        public:
            FunctionDeclarationBehavior() = default;
            virtual ~FunctionDeclarationBehavior() override = default;

            virtual void unserializeDecl(Input &input) override;

            virtual const std::string &getType() const override;
            virtual const std::string &getName() const override;

            const std::vector<std::tuple<std::string, std::string>> &getArgs() const;
            const Block &getBlock() const;

    };

}

std::ostream &operator<<(std::ostream &s, const raven::DeclarationBehavior &behavior);
std::ostream &operator<<(std::ostream &s, const raven::VariableDeclarationBehavior &behavior);
std::ostream &operator<<(std::ostream &s, const raven::FunctionDeclarationBehavior &behavior);
