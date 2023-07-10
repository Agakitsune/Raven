/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Parser
*/

#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace raven::arg {

    struct Constraint {
        std::string shrt;
        std::vector<std::string> lng;
        std::string description;
        bool optional = true;
        bool value = false;
        std::string deflt = "";
        std::string type = "string";
    };

    class Parser {
        std::string _program;
        std::vector<std::string> _args;
        std::vector<std::string> _flags;
        std::vector<std::pair<std::string, std::string>> _parameters;

        std::vector<Constraint> _constraints;

        bool _bad = false;

        public:
            Parser(const char **argv, const std::initializer_list<Constraint> &constraints);
            Parser(char **argv, const std::initializer_list<Constraint> &constraints);
            ~Parser() = default;

            bool has(std::initializer_list<std::string> flags) const;
            std::string query(std::initializer_list<std::string> flags) const;

            const std::string &program() const;
            std::vector<std::string> &args();
            const std::vector<std::string> &args() const;
            const std::vector<std::string> &flags() const;
            const std::vector<std::pair<std::string, std::string>> &parameters() const;

            bool bad() const;

            void help() const;

    };

}
