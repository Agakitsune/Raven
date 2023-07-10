/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Utils
*/

#pragma once

#include <locale>

namespace raven {

    template<class Facet>
    struct span : Facet {
        template<class... Args>
        span(Args&&... args) : Facet(std::forward<Args>(args)...) {}
        ~span() {}
    };

    struct codecvtUTF8 : span<std::codecvt<char32_t, char, std::mbstate_t>> {};

    struct entryType {
        using type = std::conditional_t<sizeof(void *) == 8, uint64_t, uint32_t>;
    };

}
