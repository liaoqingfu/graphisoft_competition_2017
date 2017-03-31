#include "Field.hpp"
#include "HexMatrix.hpp"

#include <boost/program_options.hpp>
#include <iostream>

int main() {
    hex::printEcosim(std::cout, hex::readEcosim<Field>(std::cin));
}
