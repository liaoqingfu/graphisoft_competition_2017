#include "DistanceChooser.hpp"
#include "ChooserHelper.hpp"

#include <unordered_set>

Step DistanceChooser::chooseBadStep(
    const std::vector<PotentialStep>& potentialSteps) {

    auto changedPotentialSteps = potentialSteps;

    savedWeights.clear();
    for (PotentialStep& step : changedPotentialSteps) {
        processStep(step);
    }
    return getDelegatedChooser().chooseBadStep(changedPotentialSteps);
}

Step DistanceChooser::chooseGoodStep(
    const std::vector<PotentialStep>& potentialSteps) {

    auto changedPotentialSteps = potentialSteps;

    savedWeights.clear();
    for (PotentialStep& step : changedPotentialSteps) {
        processStep(step);
    }

    return getDelegatedChooser().chooseGoodStep(changedPotentialSteps);
}

void DistanceChooser::processStep(PotentialStep& step) {
    auto key = std::make_tuple(step.getStep().pushDirection,
            step.getStep().pushPosition,
            step.getStep().pushFieldType);
    auto iterator = savedWeights.find(key);
    if (iterator != savedWeights.end()) {
        // std::cerr << "Saved weight: " << iterator->second << "\n";
        step.addWeight(iterator->second);
        step.addDebugInfo({"DistanceChooser", 0.0, iterator->second});
        return;
    }

    double weight = 0;
    {
        TemporaryStep temporaryStep{step.getSourceState(), step.getStep()};
        const Track& track = step.getSourceState().track;

        const auto& gi = step.getSourceState().gameInfo;

        double size = static_cast<double>(track.width() + track.height());
        Point ownPrincess = step.getStep().princessTarget;
        for (int opponentId = 0; opponentId < gi.numPlayers; ++opponentId) {
            if (opponentId == gi.playerId) {
                continue;
            }
            Point princess = track.getPrincess(opponentId);
            weight += (1.0 - ((std::abs(ownPrincess.x - princess.x) +
                                    std::abs(ownPrincess.y - princess.y)) /
                            static_cast<double>(size)));
        }
    }
    double result = weight * weightMultiplier;
    savedWeights.emplace(key, result);
    step.addDebugInfo({"DistanceChooser", weight, result});
    step.addWeight(result);
}
