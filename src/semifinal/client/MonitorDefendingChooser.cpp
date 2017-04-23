#include "MonitorDefendingChooser.hpp"
#include "ChooserHelper.hpp"

#include <unordered_set>

Step MonitorDefendingChooser::chooseBadStep(
    const std::vector<PotentialStep>& potentialSteps) {

    auto changedPotentialSteps = potentialSteps;

    savedWeights.clear();
    for (PotentialStep& step : changedPotentialSteps) {
        processStep(step);
    }
    return getDelegatedChooser().chooseBadStep(changedPotentialSteps);
}

Step MonitorDefendingChooser::chooseGoodStep(
    const std::vector<PotentialStep>& potentialSteps) {

    auto changedPotentialSteps = potentialSteps;

    savedWeights.clear();
    for (PotentialStep& step : changedPotentialSteps) {
        processStep(step);
    }

    return getDelegatedChooser().chooseGoodStep(changedPotentialSteps);
}

void MonitorDefendingChooser::processStep(PotentialStep& step) {
    auto key = std::make_tuple(step.step.pushDirection, step.step.pushPosition,
            step.step.pushFieldType);
    auto iterator = savedWeights.find(key);
    std::cerr << "Step " << step.step << "\n";
    if (iterator != savedWeights.end()) {
        std::cerr << "Saved weight: " << iterator->second << "\n";
        step.weight += iterator->second;
        return;
    }

    GameState newGameState{*step.sourceState, *step.targetTrack};
    newGameState.extraField = step.targetExtraField;
    const auto& gi = newGameState.gameInfo;
    const auto& opponentsInfo = *step.opponentsInfo;

    double weight = 0;
    for (int opponentId = 0; opponentId < gi.numPlayers;
         ++opponentId) {

        // skip ourself
        if (opponentId == gi.playerId) continue;

        int opponentExtraField = opponentsInfo[opponentId].extraField;
        auto nextSteps = calculatePotentialSteps(
            newGameState, opponentsInfo, opponentId, opponentExtraField);
        std::unordered_set<int> reachableMonitors;
        for (const PotentialStep& nextStep : nextSteps) {
            const auto& reachablePoints =
                nextStep.targetTrack->getReachablePoints(
                    nextStep.targetTrack->getPrincess(opponentId));

            // TODO work only with the target monitor of the opponent
            for (Point p : reachablePoints) {
                int monitor = nextStep.targetTrack->getField(p).monitor;
                if (monitor != -1) {
                    reachableMonitors.insert(monitor);
                }
            }
        }

        double w = static_cast<double>(reachableMonitors.size())
                / gi.numDisplays;
        // std::cerr << "  Player " << opponentId << ": reachable monitors = "
        //         << reachableMonitors.size() << " w=" << w << "\n";
        weight += w;
    }
    weight /= gi.numPlayers;

    double w = (1.0 - weight) * weightMultiplier;
    std::cerr << "Total weight = " << w << "\n";
    step.weight += w;
    savedWeights.emplace(key, w);
}
