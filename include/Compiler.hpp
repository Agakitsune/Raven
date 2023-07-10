/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Compiler
*/

#pragma once

#include <filesystem>
#include <unordered_map>
#include <tuple>
#include <stack>
#include <optional>

#include <execinfo.h>

#include "HsUtils.hpp"
#include "Unserializer.hpp"
#include "Input.hpp"
#include "FunctionPrototype.hpp"

#include "elf/Elf.hpp"
#include "elf/Symbol.hpp"
#include "elf/section/StrSection.hpp"
#include "elf/section/Symtab.hpp"
#include "elf/section/Text.hpp"

#include "asm/Add.hpp"
#include "asm/Sub.hpp"
#include "asm/Call.hpp"
#include "asm/Label.hpp"
#include "asm/IDiv.hpp"
#include "asm/IMul.hpp"
#include "asm/Inc.hpp"
#include "asm/Dec.hpp"
#include "asm/Xor.hpp"
#include "asm/And.hpp"
#include "asm/Or.hpp"
#include "asm/Not.hpp"
#include "asm/Neg.hpp"
#include "asm/Shift.hpp"
#include "asm/Rotate.hpp"
#include "asm/Cmp.hpp"
#include "asm/Jmp.hpp"
#include "asm/Set.hpp"
#include "asm/Syscall.hpp"
#include "asm/Ret.hpp"
#include "asm/Push.hpp"
#include "asm/Pop.hpp"
#include "asm/Convert.hpp"

#include "std/Std.hpp"

#include "exception/CompilerException.hpp"

#include "Log.hpp"

#include "arg/Parser.hpp"

namespace raven {

    struct Variable {
        std::string type;
        size_t offset;
        std::shared_ptr<Expression> value = nullptr;
        bool outOfScope = false;
    };

    struct CompilerOption {
        std::string output = "a.out";
        bool verbose = false;
    };

    enum DependencyType {
        VariableDependency,
        FunctionDependency,
        RodataDependency
    };

    struct Dependency {
        DependencyType type;
        std::string caller;
        std::string callee;
        size_t callerIndex = 0;
        size_t calleeIndex = 0;
    };

    class FunctionCompiler {
        std::unordered_map<std::string, Variable> variables;
        std::filesystem::path scope = "root";

        size_t offset = 0;
        std::stack<size_t> sizeStack;
        size_t functionSize = 0;

        std::string name;
        std::string ret;

        protected:
            Variable &getVariable(const std::string &name);
            bool hasVariable(const std::string &name);
            std::string getVariableType(const std::string &name);
            std::string getExpressionType(const Expression &exp);
            bool isIntegralType(const std::string &type);
            bool needPromotion(ASM::Register reg, const std::string &type);
            void destroyScope(const std::filesystem::path &scope);

            // variable

            std::vector<std::shared_ptr<ASM::Instruction>> loadVariable(const std::string &name, ASM::Register reg);
            std::vector<std::shared_ptr<ASM::Instruction>> storeVariable(const std::string &name, ASM::Register reg);
            std::vector<std::shared_ptr<ASM::Instruction>> createVariable(const std::string &type, const std::string &name, ASM::Register reg);

            // index

            std::vector<std::shared_ptr<ASM::Instruction>> loadIndex(const Index &index, ASM::Register reg);
            std::vector<std::shared_ptr<ASM::Instruction>> storeIndex(const Index &index, ASM::Register reg);

            // misc load
            std::vector<std::shared_ptr<ASM::Instruction>> generateOperation(const Operation &op,
                                                                    ASM::Register reg,
                                                                    const std::function<std::vector<std::shared_ptr<ASM::Instruction>>(ASM::Register, ASM::Register)> &classic,
                                                                    const std::function<std::vector<std::shared_ptr<ASM::Instruction>>(ASM::Register, const ASM::Immediate&)> &variant,
                                                                    const std::function<long(long, long)> &optimize,
                                                                    bool ignoreOptimize = false);

            std::vector<std::shared_ptr<ASM::Instruction>> loadOperation(const Operation &op, ASM::Register reg);

            std::vector<std::shared_ptr<ASM::Instruction>> loadUpdate(const Expression &exp, ASM::Register reg, bool inc);

            std::vector<std::shared_ptr<ASM::Instruction>> loadExpression(const Expression &exp, ASM::Register reg);
            std::vector<std::shared_ptr<ASM::Instruction>> logicExpression(const Expression &exp, ASM::Register reg, bool _not);
            std::vector<std::shared_ptr<ASM::Instruction>> jmpExpression(const Expression &exp, bool &ignoreBase, bool &ignoreAlternate);

            std::vector<std::shared_ptr<ASM::Instruction>> logicRegister(ASM::Register reg, bool _not);

            // handle

            std::vector<std::shared_ptr<ASM::Instruction>> handleCall(const Call &call);
            std::vector<std::shared_ptr<ASM::Instruction>> handleDeclaration(const Declaration &decl, bool preStacked = false);

            std::vector<std::shared_ptr<ASM::Instruction>> handleOperation(const Operation &op);
            std::vector<std::shared_ptr<ASM::Instruction>> handleAssignement(const Operation &op);

            std::vector<std::shared_ptr<ASM::Instruction>> handleExpression(const Expression &exp);

            std::vector<std::shared_ptr<ASM::Instruction>> handleIf(const If &if_,
                                                                    const std::string &returnType,
                                                                    bool &doesReturn,
                                                                    bool &alternateReturn);

            std::vector<std::shared_ptr<ASM::Instruction>> handleBlock(const Block &block,
                                                                        const std::string &returnType,
                                                                        bool &doesReturn,
                                                                        bool stackOverride = false);


            bool callable(const Call &call, const FunctionPrototype &prototype);

        public:
            FunctionCompiler(const std::string &name, const std::string &ret);
            ~FunctionCompiler() = default;

            std::vector<std::shared_ptr<ASM::Instruction>> compile(const Declaration &func);

    };

    std::string generateKey(const std::filesystem::path &scope, const std::string &name);
    std::string getArrayType(const std::string &type);
    std::string getArrayValueType(const std::string &type);
    std::string getGlobalExpressionType(const Expression &exp, const std::string &type = "");

    const std::pair<size_t, ASM::RegisterSize> &getMetaType(const std::string &type);
    size_t sizeofType(const std::string &type);
    ASM::AccessFactor getAccessFactor(const std::string &type);
    ASM::RegisterSize getRegisterSize(const std::string &type);

    void dumpExpression(const std::string &name,
                        const std::string &type,
                        const Expression &exp,
                        std::vector<uint8_t> &data,
                        size_t index = 0
                    );

    class Compiler {
        static std::unordered_map<std::string, elf::Symbol> _symbols;
        static std::unordered_map<std::string, FunctionPrototype> _functions;
        static std::unordered_map<std::string, elf::SectionHeader> _sectionHeaders;
        static std::unordered_map<std::string, std::shared_ptr<elf::Section>> _sections;
        static std::vector<
            std::tuple<
                std::string, // caller
                std::string, // callee
                size_t, // caller offset
                std::shared_ptr<ASM::Call> // instruction
            >
        > _calls;
        static std::vector<elf::ProgramHeader> _headers;

        static std::unordered_map<std::string, std::vector<std::function<void(size_t)>>> _rodataCallbacks;
        static std::unordered_map<std::string, Variable> _data;
        static std::unordered_map<std::string, std::vector<std::function<void(size_t)>>> _dataCallbacks;
        static std::unordered_map<std::string, Variable> _bss;
        static std::unordered_map<std::string, std::vector<std::function<void(size_t)>>> _bssCallbacks;

        static std::vector<Dependency> _dependencies;

        static CompilerOption _option;

        Compiler() = default;
        ~Compiler() = default;

        static std::vector<uint8_t> __start();

        static std::vector<Declaration> parse(std::vector<std::string> &files);
        static void setup();
        static Declaration loadSymbols(const std::vector<Declaration> &decls);

        template<typename T>
        requires std::derived_from<T, elf::Section>
        static T *getSection(const std::string &name) {
            return dynamic_cast<T*>(_sections.at(name).get());
        }

        static elf::Elf generateElf();

        public:
            static int compile(int argc, char *argv[]);

            static const CompilerOption &getOption();

            static const FunctionPrototype &getFunction(const std::string &name);
            static void functionCall(const std::string &caller, const std::string &callee, size_t offset, std::shared_ptr<ASM::Call> instruction);

            static void addRodata(const std::string &value, const std::function<void(size_t)> &callback);

            static void addDependency(const Dependency &dep);
            static Dependency &getDependenciesFromCaller(const std::string &caller);
            static std::vector<Dependency*> getDependenciesFromCallee(const std::string &callee);

            static Variable &getVariable(const std::string &name);
            static bool hasVariable(const std::string &name);
            static void addVariable(const std::string &value, const std::function<void(size_t)> &callback);

    };

}
