#ifndef SEMIFINAL_CLIENT_PRINCESSMOVINGCHOOSER_HPP
#define SEMIFINAL_CLIENT_PRINCESSMOVINGCHOOSER_HPP

#include "DelegatingChooser.hpp"
#include "GameState.hpp"
#include "PotentialStep.hpp"
#include "Track.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <unordered_map>

class PrincessMovingChooser : public DelegatingChooser {
public:
    PrincessMovingChooser(std::shared_ptr<IChooser> chooser,
            double weightMultiplier, bool overrideGoodSteps) :
            DelegatingChooser(std::move(chooser)),
            weightMultiplier(weightMultiplier),
            overrideGoodSteps(overrideGoodSteps) {}

    Step chooseGoodStep(
            const std::vector<PotentialStep>& potentialSteps) override;
    Step chooseBadStep(
            const std::vector<PotentialStep>& potentialSteps) override;

private:
    void processStep(std::vector<PotentialStep>& stepValues,
            const PotentialStep& step,
            bool isBadStep = false);
    double calculateWeight(PotentialStep& step);

    double weightMultiplier;
    bool overrideGoodSteps;
};

#endif // SEMIFINAL_CLIENT_PRINCESSMOVINGCHOOSER_HPP
