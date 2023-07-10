/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Instruction
*/

#pragma once

#include <vector>
#include <array>
#include <cstdint>
#include <optional>

#include "asm/Legacy.hpp"
#include "asm/REX.hpp"
#include "asm/SIB.hpp"
#include "asm/ModRM.hpp"

#include "exception/ASMException.hpp"

namespace raven::ASM {

    class Instruction {
        protected:
            std::array<uint8_t, 4> _legacy = {0, 0, 0, 0};
            std::optional<ModRM> _rm;
            std::optional<REX> _rex;
            std::optional<SIB> _sib;

            virtual size_t getOpcodeSize() const = 0;

            virtual void dumpOpcode(std::vector<uint8_t> &data) const = 0;
            virtual void dumpOther(std::vector<uint8_t> &data) const = 0;

        public:
            Instruction() = default;
            virtual ~Instruction() = default;

            virtual void clearLegacy();
            virtual void setLegacy(const Legacy &legacy);
            virtual void setRM(const ModRM &rm);
            virtual void setREX(const REX &rex);
            virtual void setSIB(const SIB &sib);

            virtual size_t getSize() const;

            virtual void dump(std::vector<uint8_t> &data) const;

            static void dump(std::vector<uint8_t> &data, std::vector<std::shared_ptr<Instruction>> &code);

    };

}
