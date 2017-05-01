#pragma once

#include "DelegatingChooser.hpp"
#include "GameState.hpp"
#include "PotentialStep.hpp"
#include "Track.hpp"

class OpponentPushCheckingChooser : public DelegatingChooser {
public:
    OpponentPushCheckingChooser(std::shared_ptr<IChooser> chooser,
            double weightMultiplier)
        : DelegatingChooser(std::move(chooser)),
          weightMultiplier(weightMultiplier) {
    }

    Step chooseGoodStep(
        const std::vector<PotentialStep>& potentialSteps) override;

    Step chooseBadStep(
        const std::vector<PotentialStep>& potentialSteps) override;

private:
    void processStep(PotentialStep& step);

    double weightMultiplier;
};
