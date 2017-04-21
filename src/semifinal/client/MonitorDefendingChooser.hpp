#pragma once

#include "DelegatingChooser.hpp"
#include "GameState.hpp"
#include "PotentialStep.hpp"
#include "Track.hpp"

#include <algorithm>
#include <iostream>
#include <unordered_map>

class MonitorDefendingChooser : public DelegatingChooser {
public:
    MonitorDefendingChooser(std::shared_ptr<IChooser> chooser)
        : DelegatingChooser(std::move(chooser)) {}

    Step chooseGoodStep(
        const std::vector<PotentialStep>& potentialSteps) override;

    Step chooseBadStep(
        const std::vector<PotentialStep>& potentialSteps) override;

private:
    void processStep(PotentialStep step);
};

