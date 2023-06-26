#include "HsUtils.hpp"

#include <iostream>

int main(int argc, char *argv[]) {
    hs_init(&argc, &argv);
    char test[] = "test.raven";
    uint8_t *data = reinterpret_cast<uint8_t*>(ravenSerialize(test));
    std::cout << (int)data[0] << std::endl;
    hs_exit();
}