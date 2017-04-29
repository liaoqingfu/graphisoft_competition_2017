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
    auto key = std::make_tuple(step.step.pushDirection, step.step.pushPosition,
            step.step.pushFieldType);
    auto iterator = savedWeights.find(key);
    if (iterator != savedWeights.end()) {
        // std::cerr << "Saved weight: " << iterator->second << "\n";
        step.weight += iterator->second;
        return;
    }

    GameState newGameState{*step.sourceState, *step.targetTrack};
    newGameState.extraField = step.targetExtraField;
    const auto& gi = newGameState.gameInfo;

    double weight = 0;
    double size = static_cast<double>(step.targetTrack->width() +
            step.targetTrack->height());
    Point ownPrincess = step.step.princessTarget;
    for (int opponentId = 0; opponentId < gi.numPlayers; ++opponentId) {
        if (opponentId == gi.playerId) {
            continue;
        }
        Point princess = step.targetTrack->getPrincess(opponentId);
        weight += (1.0 - ((std::abs(ownPrincess.x - princess.x) +
                                std::abs(ownPrincess.y - princess.y)) /
                        static_cast<double>(size)));
    }
    double result = weight * weightMultiplier;
    savedWeights.emplace(key, result);
    step.weight += result;
}
