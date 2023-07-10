/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Symbol
*/

#include "elf/Symbol.hpp"

namespace raven::elf {

    Symbol &Symbol::setName(const std::string &name) {
        _nameStr = name;
        return *this;
    }

    Symbol &Symbol::setName(entryType::type name) {
        _name = name;
        return *this;
    }

    Symbol &Symbol::setValue(entryType::type value) {
        _value = value;
        return *this;
    }

    Symbol &Symbol::setSize(entryType::type size) {
        _size = size;
        return *this;
    }

    Symbol &Symbol::setInfo(SymbolBinding binding, SymbolType type) {
        _info = (((binding & 0x0f) << 4) + (type & 0x0f));
        return *this;
    }

    Symbol &Symbol::setOther(uint8_t other) {
        _other = other;
        return *this;
    }

    Symbol &Symbol::setShndx(uint16_t shndx) {
        _shndx = shndx;
        return *this;
    }

    const std::string &Symbol::getName() const {
        return _nameStr;
    }

    entryType::type Symbol::getValue() const {
        return _value;
    }

    SymbolType Symbol::getType() const {
        return static_cast<SymbolType>(_info & 0x0f);
    }

    uint16_t Symbol::getShndx() const {
        return _shndx;
    }

    void Symbol::dump(std::fstream &file) {
        std::vector<uint8_t> stream;
        dump(stream);
        file.write(reinterpret_cast<const char *>(stream.data()), stream.size());
    }

    void Symbol::dump(std::vector<uint8_t> &stream) {
        if constexpr (std::is_same_v<entryType::type, uint32_t>) {
            stream.insert(stream.end(), reinterpret_cast<const uint8_t *>(&_name), reinterpret_cast<const uint8_t *>(&_name) + sizeof(_name));
            stream.insert(stream.end(), reinterpret_cast<const uint8_t *>(&_value), reinterpret_cast<const uint8_t *>(&_value) + sizeof(_value));
            stream.insert(stream.end(), reinterpret_cast<const uint8_t *>(&_size), reinterpret_cast<const uint8_t *>(&_size) + sizeof(_size));
            stream.insert(stream.end(), reinterpret_cast<const uint8_t *>(&_info), reinterpret_cast<const uint8_t *>(&_info) + sizeof(_info));
            stream.insert(stream.end(), reinterpret_cast<const uint8_t *>(&_other), reinterpret_cast<const uint8_t *>(&_other) + sizeof(_other));
            stream.insert(stream.end(), reinterpret_cast<const uint8_t *>(&_shndx), reinterpret_cast<const uint8_t *>(&_shndx) + sizeof(_shndx));
        } else {
            stream.insert(stream.end(), reinterpret_cast<const uint8_t *>(&_name), reinterpret_cast<const uint8_t *>(&_name) + sizeof(_name));
            stream.insert(stream.end(), reinterpret_cast<const uint8_t *>(&_info), reinterpret_cast<const uint8_t *>(&_info) + sizeof(_info));
            stream.insert(stream.end(), reinterpret_cast<const uint8_t *>(&_other), reinterpret_cast<const uint8_t *>(&_other) + sizeof(_other));
            stream.insert(stream.end(), reinterpret_cast<const uint8_t *>(&_shndx), reinterpret_cast<const uint8_t *>(&_shndx) + sizeof(_shndx));
            stream.insert(stream.end(), reinterpret_cast<const uint8_t *>(&_value), reinterpret_cast<const uint8_t *>(&_value) + sizeof(_value));
            stream.insert(stream.end(), reinterpret_cast<const uint8_t *>(&_size), reinterpret_cast<const uint8_t *>(&_size) + sizeof(_size));
        }
    }

}
