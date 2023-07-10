/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Convert
*/

#include "asm/Convert.hpp"

namespace raven::ASM {

    Convert::Convert() {
        update();
    }

    Convert::Convert(const ConvertType &type) : _type(type) {
        update();
    }

    void Convert::update() {
        clearLegacy();
        _rex = std::nullopt;

        switch (_type) {
            case ConvertType::CBW:
                setLegacy(OPOverride);
                opcode = 0x98;
                break;
            case ConvertType::CWDE:
                opcode = 0x98;
                break;
            case ConvertType::CDQE:
                opcode = 0x98;
                _rex = REX().W(true);
                break;
        }
    }

    size_t Convert::getOpcodeSize() const {
        return 1;
    }

    void Convert::dumpOpcode(std::vector<uint8_t> &data) const {
        data.push_back(opcode);
    }

    void Convert::dumpOther(std::vector<uint8_t> &data) const {
        (void)data;
    }

    Convert &Convert::setType(ConvertType type) {
        _type = type;
        update();
        return *this;
    }

}
