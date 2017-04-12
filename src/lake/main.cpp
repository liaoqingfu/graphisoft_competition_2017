#include "Lake.hpp"

#include <iostream>
#include <random>

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
            int /*bikeTime*/, int /*totalTime*/, std::size_t /*hash*/) {
        auto iterator = getRandomElement(usableFerries, rng,
                [this](const Ferry* ferry) {
                    int value = totalBikeTime - (ferry->skippedBikeTime);
                    return value * value;
                });
        return std::distance(usableFerries.begin(), iterator);
    }

    template<typename FerrySet>
    std::size_t chooseFerryToRemove(
            const FerrySet& usedFerries, const FerrySet& /*usableFerries*/,
            int /*bikeTime*/, int /*totalTime*/, std::size_t /*hash*/) {
        auto iterator = getRandomElement(usedFerries, rng,
                [](const Ferry* ferry) {
                    int value = ferry->skippedBikeTime;
                    return value * value;
                });
        return std::distance(usedFerries.begin(), iterator);
    }

    void initialize(const Solver<RandomFerryChooser>& solver) {
        const Problem& problem = solver.getProblem();
        totalBikeTime = std::accumulate(problem.bikePaths.begin(),
                problem.bikePaths.end(), 0);
    }
private:
    std::mt19937 rng{std::random_device{}()};
    int totalBikeTime;
    // std::mt19937 rng{123123};
};

int main() {
    RandomFerryChooser ferryChooser;
    Solver<RandomFerryChooser> solver{readInput(std::cin), ferryChooser,
            std::numeric_limits<int>::max(), boost::posix_time::seconds(9)};
    solver.findShortestPath();
    solver.solve();
    auto solution = solver.getResult();
    std::cout << solution.size() << "\n";
    for (const auto& ferry : solution) {
        std::cout << ferry.first << " " << ferry.second << "\n";
    }
    return solver.checkResult() ? 0 : 1;
}
