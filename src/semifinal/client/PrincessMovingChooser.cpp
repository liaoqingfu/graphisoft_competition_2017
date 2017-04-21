#include "PrincessMovingChooser.hpp"
#include "ChooserHelper.hpp"

Step PrincessMovingChooser::chooseGoodStep(
        const std::vector<PotentialStep>& potentialSteps) {
    if (overrideGoodSteps) {
        auto steps = potentialSteps;
        for (PotentialStep& step : steps) {
            setWeight(step);
        }
        return DelegatingChooser::chooseGoodStep(steps);
    } else {
        return DelegatingChooser::chooseGoodStep(potentialSteps);
    }
}

Step PrincessMovingChooser::chooseBadStep(
        const std::vector<PotentialStep>& potentialSteps) {
    return processPotentialSteps(potentialSteps,
            [this](auto& stepValues, const auto& step) {
                return processStep(stepValues, step);
            }, getDelegatedChooser(), "PrincessMovingChooser");
}

void PrincessMovingChooser::processStep(std::vector<PotentialStep>& stepValues,
        PotentialStep step) {
    const Track& track = *step.targetTrack;
    int playerId = step.sourceState->gameInfo.playerId;

    for (Point p : track.getReachablePoints(track.getPrincess(playerId))) {
        step.step.princessTarget = p;
        setWeight(step);
        stepValues.push_back(step);
    }
}

void PrincessMovingChooser::setWeight(PotentialStep& step) {
    const Track& track = *step.targetTrack;
    int playerId = step.sourceState->gameInfo.playerId;
    Point target = track.getMonitor(playerId);

    int size = track.width() + track.height();
    step.weight += (1.0 - (std::abs(step.step.princessTarget.x - target.x)
                + std::abs(step.step.princessTarget.y - target.y))
                        / static_cast<double>(size))
                * weightMultiplier;

}
