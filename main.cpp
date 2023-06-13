#include "HsUtils.hpp"

#include <iostream>

int main(int argc, char *argv[]) {
    hs_init(&argc, &argv);
    std::cout << test() << std::endl;
}