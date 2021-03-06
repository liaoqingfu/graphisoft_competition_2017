#pragma once

#include "DelegatingChooser.hpp"
#include "GameState.hpp"
#include "PotentialStep.hpp"
#include "Track.hpp"

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <tuple>

class MaxReachableChooser : public DelegatingChooser {
public:
    MaxReachableChooser(std::shared_ptr<IChooser> chooser,
            double weightMultiplier)
        : DelegatingChooser(std::move(chooser)),
          weightMultiplier(weightMultiplier) {}

    Step chooseGoodStep(
        const std::vector<PotentialStep>& potentialSteps) override;

    Step chooseBadStep(
        const std::vector<PotentialStep>& potentialSteps) override;

private:
    void processStep(PotentialStep& step, bool isBadStep = false);

    double weightMultiplier;
    std::map<std::tuple<Directions, int, int>, double> savedWeights;
};
