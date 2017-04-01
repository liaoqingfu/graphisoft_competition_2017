#include "HexMatrix.hpp"
#include "Field.hpp"

#include <iostream>

int main() {
    hex::printMatrix(std::cout, hex::readEcosim<Field>(std::cin));
}
