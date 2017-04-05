#include "Lake.hpp"

#include <iostream>

int main() {
    Solver solver{readInput(std::cin)};
    solver.solve();
    auto solution = solver.getResult();
    std::cout << solution.size() << "\n";
    for (const auto& ferry : solution) {
        std::cout << ferry.first << " " << ferry.second << "\n";
    }
    // TODO this is not needed for production
    return solver.checkResult() ? 0 : 1;
}
