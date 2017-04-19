#include "PrincessMovingChooser.hpp"
#include "ChooserHelper.hpp"

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
    int playerId = step.sourceState->playerId;
    Point target = track.getMonitor(playerId);

    int size = track.width() + track.height();
    for (Point p : track.getReachablePoints(track.getPrincess(playerId))) {
        step.step.princessTarget = p;
        step.weight = (size - (std::abs(p.x - target.x)
                + std::abs(p.y - target.y))) * weightMultiplier;
        stepValues.push_back(step);
    }
}
