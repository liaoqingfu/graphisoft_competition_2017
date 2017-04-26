#pragma once

#include "DelegatingChooser.hpp"
#include "GameState.hpp"
#include "PotentialStep.hpp"
#include "Track.hpp"

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <tuple>

class MonitorDefendingChooser : public DelegatingChooser {
public:
    MonitorDefendingChooser(std::shared_ptr<IChooser> chooser,
            double monitorWeightMultiplier,
            double reachabilityWeightMultiplier)
        : DelegatingChooser(std::move(chooser)),
          monitorWeightMultiplier(monitorWeightMultiplier),
          reachabilityWeightMultiplier(reachabilityWeightMultiplier) {}

    Step chooseGoodStep(
        const std::vector<PotentialStep>& potentialSteps) override;

    Step chooseBadStep(
        const std::vector<PotentialStep>& potentialSteps) override;

private:
    void processStep(PotentialStep& step);

    double monitorWeightMultiplier;
    std::map<std::tuple<Directions, int, int>, double> savedWeights;
    double reachabilityWeightMultiplier;
};

