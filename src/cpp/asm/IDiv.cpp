/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** IDiv
*/

#include "asm/IDiv.hpp"

namespace raven::ASM {

    void IDiv::update() {

        clearLegacy();

        switch (_register.size) {
            case S8:
                opcode = 0xf6;
                _rm = ModRM().setMod(Mod::Mod03).setRm(_register).setReg(EDI);
                if (_register.useREX)
                    _rex = REX().B(_register.extend);
                else
                    _rex = std::nullopt;
                break;
            case S16:
                setLegacy(OPOverride);
                opcode = 0xf7;
                _rm = ModRM().setMod(Mod::Mod03).setRm(_register).setReg(EDI);
                if (_register.useREX)
                    _rex = REX().B(_register.extend);
                else
                    _rex = std::nullopt;
                break;
            case S32:
                opcode = 0xf7;
                _rm = ModRM().setMod(Mod::Mod03).setRm(_register).setReg(EDI);
                if (_register.useREX)
                    _rex = REX().B(_register.extend);
                else
                    _rex = std::nullopt;
                break;
            case S64:
                opcode = 0xf7;
                _rm = ModRM().setMod(Mod::Mod03).setRm(_register).setReg(EDI);
                _rex = REX().W(true).B(_register.extend);
                break;
        }

    }

    size_t IDiv::getOpcodeSize() const {
        return 1;
    }

    std::vector<std::shared_ptr<Instruction>> IDiv::div(const Register &dst, const Register &src) {
        std::vector<std::shared_ptr<Instruction>> instructions;
        std::vector<std::shared_ptr<Instruction>> tmp;

        auto div = std::make_shared<IDiv>();

        switch (dst.size) {
            case S8:
                tmp = Move::safeMove(AX, dst);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                div->setRegister(src);
                instructions.push_back(div);
                tmp = Move::safeMove(dst, AL);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case S16:
                instructions.push_back(std::make_shared<Xor>(DX, DX));
                tmp = Move::safeMove(AX, dst);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                div->setRegister(src);
                instructions.push_back(div);
                tmp = Move::safeMove(dst, AX);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case S32:
                instructions.push_back(std::make_shared<Xor>(EDX, EDX));
                tmp = Move::safeMove(EAX, dst);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                div->setRegister(src);
                instructions.push_back(div);
                tmp = Move::safeMove(dst, EAX);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case S64:
                instructions.push_back(std::make_shared<Xor>(RDX, RDX));
                tmp = Move::safeMove(RAX, dst);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                div->setRegister(src);
                instructions.push_back(div);
                tmp = Move::safeMove(dst, RAX);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
        }

        return instructions;
    }

    std::vector<std::shared_ptr<Instruction>> IDiv::div(const Register &dst, const int64_t &src) {
        std::vector<std::shared_ptr<Instruction>> instructions;
        std::vector<std::shared_ptr<Instruction>> tmp;

        auto div = std::make_shared<IDiv>();

        instructions.push_back(std::make_shared<Mov>(RCX, src));

        switch (dst.size) {
            case S8:
                tmp = Move::safeMove(AX, dst);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                div->setRegister(CL);
                instructions.push_back(div);
                tmp = Move::safeMove(dst, AL);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case S16:
                instructions.push_back(std::make_shared<Xor>(DX, DX));
                tmp = Move::safeMove(AX, dst);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                div->setRegister(CL);
                instructions.push_back(div);
                tmp = Move::safeMove(dst, AX);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case S32:
                instructions.push_back(std::make_shared<Xor>(EDX, EDX));
                tmp = Move::safeMove(EAX, dst);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                div->setRegister(CL);
                instructions.push_back(div);
                tmp = Move::safeMove(dst, EAX);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case S64:
                instructions.push_back(std::make_shared<Xor>(RDX, RDX));
                tmp = Move::safeMove(RAX, dst);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                div->setRegister(CL);
                instructions.push_back(div);
                tmp = Move::safeMove(dst, RAX);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
        }

        return instructions;
    }

    std::vector<std::shared_ptr<Instruction>> IDiv::mod(const Register &dst, const Register &src) {
        std::vector<std::shared_ptr<Instruction>> instructions;
        std::vector<std::shared_ptr<Instruction>> tmp;

        auto div = std::make_shared<IDiv>();

        switch (dst.size) {
            case S8:
                tmp = Move::safeMove(AX, dst);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                div->setRegister(src);
                instructions.push_back(div);
                tmp = Move::safeMove(dst, AH);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case S16:
                instructions.push_back(std::make_shared<Xor>(DX, DX));
                tmp = Move::safeMove(AX, dst);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                div->setRegister(src);
                instructions.push_back(div);
                tmp = Move::safeMove(dst, DX);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case S32:
                instructions.push_back(std::make_shared<Xor>(EDX, EDX));
                tmp = Move::safeMove(EAX, dst);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                div->setRegister(src);
                instructions.push_back(div);
                tmp = Move::safeMove(dst, EDX);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case S64:
                instructions.push_back(std::make_shared<Xor>(RDX, RDX));
                tmp = Move::safeMove(RAX, dst);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                div->setRegister(src);
                instructions.push_back(div);
                tmp = Move::safeMove(dst, RDX);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
        }

        return instructions;
    }

    std::vector<std::shared_ptr<Instruction>> IDiv::mod(const Register &dst, const int64_t &src) {
        std::vector<std::shared_ptr<Instruction>> instructions;
        std::vector<std::shared_ptr<Instruction>> tmp;

        auto div = std::make_shared<IDiv>();

        instructions.push_back(std::make_shared<Mov>(RCX, src));

        switch (dst.size) {
            case S8:
                tmp = Move::safeMove(AX, dst);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                div->setRegister(CL);
                instructions.push_back(div);
                tmp = Move::safeMove(dst, AH);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case S16:
                instructions.push_back(std::make_shared<Xor>(DX, DX));
                tmp = Move::safeMove(AX, dst);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                div->setRegister(CX);
                instructions.push_back(div);
                tmp = Move::safeMove(dst, DX);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case S32:
                instructions.push_back(std::make_shared<Xor>(EDX, EDX));
                tmp = Move::safeMove(EAX, dst);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                div->setRegister(ECX);
                instructions.push_back(div);
                tmp = Move::safeMove(dst, EDX);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
            case S64:
                instructions.push_back(std::make_shared<Xor>(RDX, RDX));
                tmp = Move::safeMove(RAX, dst);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                div->setRegister(RCX);
                instructions.push_back(div);
                tmp = Move::safeMove(dst, RDX);
                instructions.insert(instructions.end(), tmp.begin(), tmp.end());
                break;
        }

        return instructions;
    }

    void IDiv::dumpOpcode(std::vector<uint8_t> &data) const {
        if (opcode == 0)
            throw exception::ASMException("Invalid IDiv opcode, did you see some values ?");
        data.push_back(opcode);
    }

    void IDiv::dumpOther(std::vector<uint8_t> &data) const {
        (void)data;
    }

    IDiv &IDiv::setRegister(const Register &reg) {
        _register = reg;
        update();
        return *this;
    }

}
