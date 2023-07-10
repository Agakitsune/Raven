/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Parser
*/

#include "arg/Parser.hpp"

namespace raven::arg {

    Parser::Parser(const char **argv, const std::initializer_list<Constraint> &constraints) {
        _constraints = constraints;
        if (argv[0] == nullptr)
            return;
        _program = argv[0];
        size_t index = 1;
        while (argv[index]) {
            std::string arg = argv[index];
            bool _found = false;
            if (arg.starts_with("--")) {
                arg = arg.substr(2);
                for (auto &constraint : constraints) {
                    for (auto &longOpts : constraint.lng) {
                        if (longOpts == arg) {
                            if (constraint.value) {
                                if (argv[index + 1] == nullptr) {
                                    if (constraint.deflt.empty()) {
                                        std::cerr << "Missing value for option " + arg << std::endl;
                                        _bad = true;
                                        return;
                                    } else {
                                        _parameters.emplace_back(arg, constraint.deflt);
                                        _found = true;
                                        break;
                                    }
                                }
                                std::string value = argv[index + 1];
                                if (value.starts_with("-")) {
                                    std::cerr << "Missing value for option " + arg << std::endl;
                                    _bad = true;
                                    return;
                                }
                                _parameters.emplace_back(arg, value);
                                _found = true;
                                index++;
                            } else {
                                _flags.push_back(arg);
                                _found = true;
                            }
                            break;
                        }
                    }
                    if (_found)
                        break;
                    continue;
                }
                if (!_found) {
                    std::cerr << "Unknown option " + arg << std::endl;
                    _bad = true;
                    return;
                }
            } else if (arg.starts_with("-")) {
                arg = arg.substr(1);
                for (auto &constraint : constraints) {
                    for (auto &shortOpts : constraint.shrt) {
                        if (arg.find(shortOpts) != std::string::npos) {
                            std::string flag = arg.substr(arg.find(shortOpts), 1);
                            if (constraint.value) {
                                if (arg.size() != 1) {
                                    if (constraint.deflt.empty()) {
                                        std::cerr << "Option " + flag  << " need an argument"<< std::endl;
                                        _bad = true;
                                        return;
                                    } else {
                                        _parameters.emplace_back(flag, constraint.deflt);
                                        _found = true;
                                        break;
                                    }
                                }
                                if (argv[index + 1] == nullptr) {
                                    if (constraint.deflt.empty()) {
                                        std::cerr << "Missing value for option " + flag << std::endl;
                                        _bad = true;
                                        return;
                                    } else {
                                        _parameters.emplace_back(flag, constraint.deflt);
                                        _found = true;
                                        break;
                                    }
                                }
                                std::string value = argv[index + 1];
                                if (value.starts_with("-")) {
                                    std::cerr << "Missing value for option " + flag << std::endl;
                                    _bad = true;
                                    return;
                                }
                                _parameters.emplace_back(flag, value);
                                _found = true;
                                index++;
                            } else {
                                _flags.push_back(flag);
                                _found = true;
                            }
                        }
                    }
                }
                if (!_found) {
                    std::cerr << "Unknown option " + arg << std::endl;
                    _bad = true;
                    return;
                }
            } else {
                _args.push_back(arg);
            }
            index++;
        }

        for (auto &constraints : constraints) {
            if (!constraints.optional) {
                bool _found = false;
                for (auto &flag : _flags) {
                    for (auto &shortOpts : constraints.shrt) {
                        if (flag.find(shortOpts) != std::string::npos) {
                            _found = true;
                            break;
                        }
                    }
                    if (_found)
                        break;
                    for (auto &longOpts : constraints.lng) {
                        if (flag == longOpts) {
                            _found = true;
                            break;
                        }
                    }
                    if (_found)
                        break;
                }
                if (!_found) {
                    std::cerr << "Missing option " + constraints.shrt << std::endl;
                    _bad = true;
                    return;
                }
            }
        }
    }

    Parser::Parser(char **argv, const std::initializer_list<Constraint> &constraints) : Parser((const char **)argv, constraints) { }

    bool Parser::has(std::initializer_list<std::string> flags) const {
        for (auto &flag : flags) {
            for (auto &f : _flags) {
                if (f == flag)
                    return true;
            }
        }
        for (auto &parameters : _parameters) {
            for (auto &flag : flags) {
                if (parameters.first == flag)
                    return true;
            }
        }
        return false;
    }

    std::string Parser::query(std::initializer_list<std::string> flags) const {
        for (auto &parameters : _parameters) {
            for (auto &flag : flags) {
                if (parameters.first == flag)
                    return parameters.second;
            }
        }
        return "";
    }

    const std::string &Parser::program() const {
        return _program;
    }

    std::vector<std::string> &Parser::args() {
        return _args;
    }

    const std::vector<std::string> &Parser::args() const {
        return _args;
    }

    const std::vector<std::string> &Parser::flags() const {
        return _flags;
    }

    const std::vector<std::pair<std::string, std::string>> &Parser::parameters() const {
        return _parameters;
    }

    bool Parser::bad() const {
        return _bad;
    }

    void Parser::help() const {
                        for (auto &constraint : _constraints) {
            std::cout << "  ";
            if (!constraint.shrt.empty())
                std::cout << "-" << constraint.shrt << ", ";
            else
                std::cout << "    ";
            std::cout << "--" << constraint.lng[0];
            if (constraint.value)
                std::cout << " <" << constraint.type << ">";
                                }
    }

}
