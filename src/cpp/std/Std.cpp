/*
** EPITECH PROJECT, 2023
** Raven
** File description:
** Std
*/

#include "std/Std.hpp"

#include <iostream>

namespace raven::stdlib {

    std::vector<elf::Symbol> getStandardLibrarySymbols() {
        static std::vector<std::string> names = {
            "__allocateString",
            "__deallocateString",
            "__copyString",
            "__displayString",
            "read",
            "write",
            "open",
            "close",
            "exit",
            "length",
            "print",
            "println"
        };

        static std::vector<size_t> sizes = {
            56,
            29,
            57,
            36,
            40,
            18,
            18,
            18,
            9,
            32,
            14,
            51
        };

        static std::vector<elf::Symbol> symbols;
        size_t value = 0x400000;
        for (size_t i = 0; i < names.size(); ++i) {
            symbols.push_back(
                elf::Symbol().setName(names[i])
                    .setOther(raven::elf::Default)
                    .setSize(0)
                    .setInfo(raven::elf::BLocal)
                    .setValue(value)
            );
            value += sizes[i];
        };
        return symbols;
    }

    std::vector<uint8_t> getStandardLibrary() {
        std::vector<uint8_t> lib;
        uint8_t *data = __allocateString();
        lib.insert(lib.end(), data, &data[56]);
        data = __deallocateString();
        lib.insert(lib.end(), data, &data[29]);
        data = __copyString();
        lib.insert(lib.end(), data, &data[57]);
        data = __displayString();
        lib.insert(lib.end(), data, &data[36]);
        data = read();
        lib.insert(lib.end(), data, &data[40]);
        data = write();
        lib.insert(lib.end(), data, &data[18]);
        data = open();
        lib.insert(lib.end(), data, &data[18]);
        data = close();
        lib.insert(lib.end(), data, &data[18]);
        data = exit();
        lib.insert(lib.end(), data, &data[9]);
        data = length();
        lib.insert(lib.end(), data, &data[32]);
        data = print();
        lib.insert(lib.end(), data, &data[14]);
        data = println();
        lib.insert(lib.end(), data, &data[51]);
        return lib;
    }

    std::vector<FunctionPrototype> getStandardLibraryDeclarations() {
        static std::vector<FunctionPrototype> decls = {
            FunctionPrototype("String", "read", {"Integer", "Integer"}),
            FunctionPrototype("Integer", "write", {"Integer", "String"}),
            FunctionPrototype("Integer", "open", {"String", "Integer", "Integer"}),
            FunctionPrototype("Integer", "close", {"Integer"}),
            FunctionPrototype("Integer", "exit", {"Integer"}),
            FunctionPrototype("Integer", "length", {"String"}),
            FunctionPrototype("Integer", "print", {"String"}),
            FunctionPrototype("Integer", "println", {"String"}),
        };
        return decls;
    }

    uint8_t *__allocateString() {
        static uint8_t data[] = {
            0x55,                                       // push rbp
            0x48, 0x89, 0xe5,                           // mov rbp, rsp

            0x48, 0xC7, 0xC0, 0x09, 0x00, 0x00, 0x00,   // mov rax, 9 (mmap)
            0x48, 0x89, 0xFE,                           // mov rsi, rdi (load length)
            0x48, 0xC7, 0xC7, 0x00, 0x00, 0x00, 0x00,   // mov rdi, 0 (NULL)
            0x48, 0xC7, 0xC2, 0x03, 0x00, 0x00, 0x00,   // mov rdx, 0x03 (Read | Write)
            0x49, 0xC7, 0xC2, 0x22, 0x00, 0x00, 0x00,   // mov r10, 0x22 (Private | Anonymous)
            0x49, 0xC7, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF,   // mov r8, -1
            0x49, 0xC7, 0xC1, 0x00, 0x00, 0x00, 0x00,   // mov r9, 0
            0x0F, 0x05,                                 // syscall

            0x48, 0x89, 0xec,                           // mov rsp, rbp
            0x5d,                                       // pop rbp
            0xc3                                        // ret
        };
        return data;
    }

    uint8_t *__deallocateString() {
        static uint8_t data[] = {
            0x55,                                       // push rbp
            0x48, 0x89, 0xe5,                           // mov rbp, rsp

            0xe8, 0xd8, 0x00, 0x00, 0x00,               // call length

            0x48, 0x89, 0xC6,                           // mov rsi, rax
            0x48, 0xFF, 0xC6,                           // inc rsi

            0x48, 0xC7, 0xC0, 0x0b, 0x00, 0x00, 0x00,   // mov rax, 11 (munmap)
            0x0F, 0x05,                                 // syscall

            0x48, 0x89, 0xec,                           // mov rsp, rbp
            0x5d,                                       // pop rbp
            0xc3                                        // ret
        };
        return data;
    }

    uint8_t *__copyString() {
        static uint8_t data[] = {
            0x55,                                       // push rbp
            0x48, 0x89, 0xe5,                           // mov rbp, rsp

            0x48, 0x89, 0xFE,                           // mov rsi, rdi
            0x48, 0x31, 0xC9,                           // xor rcx, rcx

            0xe8, 0x8f, 0x00, 0x00, 0x00,               // call length
            0x48, 0x89, 0xC7,                           // mov rdi, rax
            0x48, 0xFF, 0xC7,                           // inc rdi

            0xe8, 0x91, 0xff, 0xff, 0xff,               // call __allocateString
            0x48, 0x89, 0xC7,                           // mov rdi, rax

            // loop
            0x80, 0x3c, 0x0e, 0x00,                     // cmp byte ptr [rsi + rcx], 0
            0x74, 0x0d,                                 // je end
            0x44, 0x8a, 0x04, 0x0e,                     // mov r8b, byte ptr [rsi + rcx]
            0x44, 0x88, 0x04, 0x0f,                     // mov byte ptr [rdi + rcx], r8b
            0x48, 0xff, 0xc1,                           // inc rcx
            0xeb, 0xed,                                 // jmp loop
            // end
            0xc6, 0x04, 0x0e, 0x00,                     // mov byte ptr [rdi + rcx], 0

            0x48, 0x89, 0xec,                           // mov rsp, rbp
            0x5d,                                       // pop rbp
            0xc3                                        // ret
        };
        return data;
    }

    uint8_t *__displayString() {
        static uint8_t data[] = {
            0x55,                                       // push rbp
            0x48, 0x89, 0xe5,                           // mov rbp, rsp

            0xe8, 0x82, 0x00, 0x00, 0x00,               // call length

            0x48, 0x89, 0xC2,                           // mov rdx, rax
            0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00,   // mov rax, 1 (write)
            0x48, 0x89, 0xfe,                           // mov rsi, rdi
            0x48, 0xC7, 0xC7, 0x01, 0x00, 0x00, 0x00,   // mov rdi, 1 (stdout)
            0x0F, 0x05,                                 // syscall

            0x48, 0x89, 0xec,                           // mov rsp, rbp
            0x5d,                                       // pop rbp
            0xc3                                        // ret
        };
        return data;
    }

    uint8_t *read() {
        static uint8_t data[] = {
            0x55,                                       // push rbp
            0x48, 0x89, 0xe5,                           // mov rbp, rsp
            0x57,                                       // push rdi
            0x48, 0x89, 0xF7,                           // mov rdi, rsi
            0x48, 0xFF, 0xC7,                           // inc rdi

            0xe8, 0x3e, 0xff, 0xff, 0xff,               // call __allocateString

            0x5f,                                       // pop rdi
            0x48, 0x89, 0xF2,                           // mov rdx, rsi
            0x48, 0x89, 0xC6,                           // mov rsi, rax
            0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00,   // mov rax, 0 (read)
            0x0F, 0x05,                                 // syscall

            0x48, 0x89, 0xF0,                           // mov rax, rsi

            0x48, 0x89, 0xec,                           // mov rsp, rbp
            0x5d,                                       // pop rbp
            0xc3                                        // ret
        };
        return data;
    }

    uint8_t *write() {
        static uint8_t data[] = {
            0x55,                                       // push rbp
            0x48, 0x89, 0xe5,                           // mov rbp, rsp
            0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00,
            0x0F, 0x05,
            0x48, 0x89, 0xec,                           // mov rsp, rbp
            0x5d,                                       // pop rbp
            0xc3                                        // ret
        };
        return data;
    }

    uint8_t *open() {
        static uint8_t data[] = {
            0x55,                                       // push rbp
            0x48, 0x89, 0xe5,                           // mov rbp, rsp
            0x48, 0xC7, 0xC0, 0x02, 0x00, 0x00, 0x00,
            0x0F, 0x05,
            0x48, 0x89, 0xec,                           // mov rsp, rbp
            0x5d,                                       // pop rbp
            0xc3                                        // ret
        };
        return data;
    }

    uint8_t *close() {
        static uint8_t data[] = {
            0x55,                                       // push rbp
            0x48, 0x89, 0xe5,                           // mov rbp, rsp
            0x48, 0xC7, 0xC0, 0x03, 0x00, 0x00, 0x00,
            0x0F, 0x05,
            0x48, 0x89, 0xec,                           // mov rsp, rbp
            0x5d,                                       // pop rbp
            0xc3                                        // ret
        };
        return data;
    }

    uint8_t *exit() {
        static uint8_t data[] = {
            0x48, 0xC7, 0xC0, 0x3c, 0x00, 0x00, 0x00,   // mov rax, 60 (exit)
            0x0F, 0x05                                  // syscall
        };
        return data;
    }

    uint8_t *length() {
        static uint8_t data[] = {
            0x55,                       // push rbp
            0x48, 0x89, 0xe5,           // mov rbp, rsp

            0x48, 0x31, 0xc9,           // xor rcx, rcx
            0x48, 0x83, 0xff, 0x00,     // cmp rdi, 0
            0x74, 0x0b,                 // je end

            // loop
            0x80, 0x3c, 0x0f, 0x00,     // cmp byte ptr [rdi + rcx], 0
            0x74, 0x05,                 // je end
            0x48, 0xff, 0xc1,           // inc rcx
            0xeb, 0xf5,                 // jmp loop

            // end
            0x48, 0x89, 0xc8,           // mov rax, rcx
            0x48, 0x89, 0xec,           // mov rsp, rbp
            0x5d,                       // pop rbp
            0xc3                        // ret
        };
        return data;
    }

    uint8_t *print() {
        static uint8_t data[] = {
            0x55,                           // push rbp
            0x48, 0x89, 0xe5,               // mov rbp, rsp

            0xe8, 0x4c, 0xff, 0xff, 0xff,   // call __displayString

            0x48, 0x89, 0xec,               // mov rsp, rbp
            0x5d,                           // pop rbp
            0xc3                            // ret
        };
        return data;
    }

    uint8_t *println() {
        static uint8_t data[] = {
            0x55,                                       // push rbp
            0x48, 0x89, 0xe5,                           // mov rbp, rsp

            0x48, 0xFF, 0xCC,                           // dec rsp
            0xC6, 0x45, 0xFF, 0x0A,                     // mov byte ptr [rbp - 1], 0x0A

            0xe8, 0x37, 0xff, 0xff, 0xff,               // call __displayString

            0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00,   // mov rax, 1 (write)
            0x48, 0x8D, 0x75, 0xFF,                     // lea rsi, [rbp - 1]
            0x48, 0xC7, 0xC7, 0x01, 0x00, 0x00, 0x00,   // mov rdi, 1 (stdout)
            0x48, 0xc7, 0xc2, 0x01, 0x00, 0x00, 0x00,   // mov rdx, 1
            0x0F, 0x05,                                 // syscall

            0x48, 0xFF, 0xC4,                           // inc rsp

            0x48, 0x89, 0xec,                           // mov rsp, rbp
            0x5d,                                       // pop rbp
            0xc3                                        // ret
        };
        return data;
    }

}
