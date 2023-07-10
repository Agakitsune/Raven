/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Access
*/

#include "asm/Access.hpp"

namespace raven::ASM {

    Access::Access(const Register &value, const AccessFactor &factor) {
        _index = value;
        _fac = factor;
    }

    Access::Access(const Immediate &value, const AccessFactor &factor) {
        switch (factor) {
            case AccessFactor::Factor1:
                _index = Immediate(value.getValue());
                break;
            case AccessFactor::Factor2:
                _index = Immediate(value.getValue() * 2);
                break;
            case AccessFactor::Factor4:
                _index = Immediate(value.getValue() * 4);
                break;
            case AccessFactor::Factor8:
                _index = Immediate(value.getValue() * 8);
                break;
        }
    }

    Access &Access::setValue(const Register &value) {
        _index = value;
        return *this;
    }

    Access &Access::setValue(const Immediate &value) {
        switch (_fac) {
            case AccessFactor::Factor1:
                _index = Immediate(value.getValue()).setSize(value.getImmediateSize());
                break;
            case AccessFactor::Factor2:
                _index = Immediate(value.getValue() * 2).setSize(value.getImmediateSize());
                break;
            case AccessFactor::Factor4:
                _index = Immediate(value.getValue() * 4).setSize(value.getImmediateSize());
                break;
            case AccessFactor::Factor8:
                _index = Immediate(value.getValue() * 8).setSize(value.getImmediateSize());
                break;
        }
        _fac = AccessFactor::Factor1;
        if (_offset.index() == 2) {
            _index = Immediate(std::get<Immediate>(_index).getValue() + std::get<Immediate>(_offset).getValue()).setSize(value.getImmediateSize());
            _offset = std::monostate{};
        }
        return *this;
    }

    Access &Access::setFactor(const AccessFactor &factor) {
        if (_index.index() == 1) {
            switch (factor) {
                case AccessFactor::Factor1:
                    _index = Immediate(std::get<Immediate>(_index).getValue());
                    break;
                case AccessFactor::Factor2:
                    _index = Immediate(std::get<Immediate>(_index).getValue() * 2);
                    break;
                case AccessFactor::Factor4:
                    _index = Immediate(std::get<Immediate>(_index).getValue() * 4);
                    break;
                case AccessFactor::Factor8:
                    _index = Immediate(std::get<Immediate>(_index).getValue() * 8);
                    break;
            }
        } else {
            _fac = factor;
        }
        return *this;
    }

    Access &Access::setOffset(const Register &offset) {
        _offset = offset;
        return *this;
    }

    Access &Access::setOffset(const Immediate &offset) {
        if (_index.index() == 1) {
            _index = Immediate(std::get<Immediate>(_index).getValue() + offset.getValue());
            _offset = std::monostate{};
        } else {
            _offset = offset;
        }
        return *this;
    }

    const std::variant<Register, Immediate> &Access::getValue() const {
        return _index;
    }

    AccessFactor Access::getAccessFactor() const {
        return _fac;
    }

    const std::variant<std::monostate, Register, Immediate> &Access::getOffset() const {
        return _offset;
    }

}
