#ifndef SEMIFINAL_RANDOM_CLIENT_RANDOMCHOOSER_HPP
#define SEMIFINAL_RANDOM_CLIENT_RANDOMCHOOSER_HPP

#include "IChooser.hpp"
#include "PotentialStep.hpp"

#include <random>

class RandomChooser : public IChooser {
public:
    RandomChooser(std::mt19937& rng) : rng(rng) {}

    Step chooseGoodStep(
            const std::vector<PotentialStep>& potentialSteps) override {
        return chooseStep(potentialSteps);
    }

    Step chooseBadStep(
            const std::vector<PotentialStep>& potentialSteps) override {
        return chooseStep(potentialSteps);
    }

private:
    const Step& chooseStep(
            const std::vector<PotentialStep>& potentialSteps) {
        std::uniform_int_distribution<std::size_t> distribution{
                0, potentialSteps.size() - 1};
        return potentialSteps[distribution(rng)].step;
    }

    std::mt19937& rng;
};

#endif // SEMIFINAL_RANDOM_CLIENT_RANDOMCHOOSER_HPP
