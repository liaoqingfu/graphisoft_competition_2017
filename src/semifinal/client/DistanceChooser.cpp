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
    const Track& targetTrack = step.getTargetTrack();
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

    GameState newGameState{step.getSourceState(), targetTrack};
    newGameState.extraField = step.getTargetExtraField();
    const auto& gi = newGameState.gameInfo;

    double weight = 0;
    double size = static_cast<double>(targetTrack.width() +
            targetTrack.height());
    Point ownPrincess = step.getStep().princessTarget;
    for (int opponentId = 0; opponentId < gi.numPlayers; ++opponentId) {
        if (opponentId == gi.playerId) {
            continue;
        }
        Point princess = targetTrack.getPrincess(opponentId);
        weight += (1.0 - ((std::abs(ownPrincess.x - princess.x) +
                                std::abs(ownPrincess.y - princess.y)) /
                        static_cast<double>(size)));
    }
    double result = weight * weightMultiplier;
    savedWeights.emplace(key, result);
    step.addDebugInfo({"DistanceChooser", weight, result});
    step.addWeight(result);
}
