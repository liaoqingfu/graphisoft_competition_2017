#ifndef SEMIFINAL_RANDOM_CLIENT_RANDOMCHOOSER_HPP
#define SEMIFINAL_RANDOM_CLIENT_RANDOMCHOOSER_HPP

#include "HeuristicSolver.hpp"

#include <random>

class RandomChooser {
public:
    const Step& chooseGoodStep(
            const std::vector<PotentialStep>& potentialSteps) {
        return chooseStep(potentialSteps);
    }

    const Step& chooseBadStep(
            const std::vector<PotentialStep>& potentialSteps) {
        return chooseStep(potentialSteps);
    }

private:
    const Step& chooseStep(
            const std::vector<PotentialStep>& potentialSteps) {
        std::uniform_int_distribution<std::size_t> distribution{
                0, potentialSteps.size() - 1};
        return potentialSteps[distribution(rng)].step;
    }

    std::mt19937 rng{std::random_device{}()};
};

#endif // SEMIFINAL_RANDOM_CLIENT_RANDOMCHOOSER_HPP
