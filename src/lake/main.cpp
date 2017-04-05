#include "Lake.hpp"

#include <iostream>

template<typename Container, typename Function, typename RNG>
typename Container::const_iterator getRandomElement(const Container& elements,
        RNG& rng, const Function& weightFunction) {
    auto range = elements | boost::adaptors::transformed(weightFunction);
    std::discrete_distribution<std::size_t> distribution{
            range.begin(), range.end()};
    return elements.begin() + distribution(rng);
}

class RandomFerryChooser {
public:
    template<typename FerrySet>
    std::size_t chooseFerryToAdd(
            const FerrySet& /*usedFerries*/, const FerrySet& usableFerries,
            int /*bikeTime*/, int /*totalTime*/) {
        auto iterator = getRandomElement(usableFerries, rng,
                [](const Ferry* ferry) {
                    return 1.0 / (ferry->skippedBikeTime);
                });
        return std::distance(usableFerries.begin(), iterator);
    }

    template<typename FerrySet>
    std::size_t chooseFerryToRemove(
            const FerrySet& usedFerries, const FerrySet& /*usableFerries*/,
            int /*bikeTime*/, int /*totalTime*/) {
        auto iterator = getRandomElement(usedFerries, rng,
                [](const Ferry* ferry) {
                    return ferry->skippedBikeTime;
                });
        return std::distance(usedFerries.begin(), iterator);
    }

private:
    std::mt19937 rng{std::random_device{}()};
    // std::mt19937 rng{123123};
};

int main() {
    Solver<RandomFerryChooser> solver{readInput(std::cin),
            RandomFerryChooser{}};
    solver.findShortestPath();
    solver.solve();
    auto solution = solver.getResult();
    std::cout << solution.size() << "\n";
    for (const auto& ferry : solution) {
        std::cout << ferry.first << " " << ferry.second << "\n";
    }
    // TODO this is not needed for production
    return solver.checkResult() ? 0 : 1;
}
