#pragma once

#include "DelegatingChooser.hpp"
#include "GameState.hpp"
#include "PotentialStep.hpp"
#include "Track.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <unordered_map>

// Multi Monitor Manhatten
class MMMChooser : public DelegatingChooser {
public:
    MMMChooser(std::shared_ptr<IChooser> chooser,
            double weightMultiplier) :
            DelegatingChooser(std::move(chooser)),
            weightMultiplier(weightMultiplier) {}

    Step chooseGoodStep(
            const std::vector<PotentialStep>& potentialSteps) override;
    Step chooseBadStep(
            const std::vector<PotentialStep>& potentialSteps) override;

private:
    void processStep(PotentialStep& step);
    double weightMultiplier;
};
