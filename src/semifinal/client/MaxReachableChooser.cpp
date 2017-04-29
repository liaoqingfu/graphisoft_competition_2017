#include "MaxReachableChooser.hpp"
#include "ChooserHelper.hpp"

#include <unordered_set>

Step MaxReachableChooser::chooseBadStep(
    const std::vector<PotentialStep>& potentialSteps) {

    auto changedPotentialSteps = potentialSteps;

    savedWeights.clear();
    for (PotentialStep& step : changedPotentialSteps) {
        processStep(step, true);
    }
    return getDelegatedChooser().chooseBadStep(changedPotentialSteps);
}

Step MaxReachableChooser::chooseGoodStep(
    const std::vector<PotentialStep>& potentialSteps) {

    auto changedPotentialSteps = potentialSteps;

    savedWeights.clear();
    for (PotentialStep& step : changedPotentialSteps) {
        processStep(step);
    }

    return getDelegatedChooser().chooseGoodStep(changedPotentialSteps);
}

void MaxReachableChooser::processStep(PotentialStep& step,
        bool isBadStep) {
    auto key = std::make_tuple(step.getStep().pushDirection,
            step.getStep().pushPosition, step.getStep().pushFieldType);
    auto iterator = savedWeights.find(key);
    if (iterator != savedWeights.end()) {
        // std::cerr << "Saved weight: " << iterator->second << "\n";
        step.addWeight(iterator->second);
        step.debugInfo.push_back(PotentialStep::DebugInfo{"MaxReachableChooser",
                iterator->second / weightMultiplier, iterator->second});
        return;
    }

    GameState newGameState = step.createNewGameState();
    const auto& gi = newGameState.gameInfo;

    // std::cerr << "Step " << step.step << "\n";

    const Track& targetTrack = step.getTargetTrack();
    const auto& reachablePoints = targetTrack.getReachablePoints(
            targetTrack.getPrincess(gi.playerId));
    double w = static_cast<double>(reachablePoints.size())
            / (gi.width * gi.height);

    // std::cerr << "Total weight = " << w << "\n";
    double ww = w * weightMultiplier;
    if (isBadStep) {
        ww *= 2;
    }
    step.addWeight(ww);
    step.debugInfo.push_back(PotentialStep::DebugInfo{"MaxReachableChooser", w, ww});
    // std::cerr << "Total weight = " << w << "\n";
    savedWeights.emplace(key, ww);
}
