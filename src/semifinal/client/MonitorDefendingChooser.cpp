#include "MonitorDefendingChooser.hpp"
#include "ChooserHelper.hpp"

Step MonitorDefendingChooser::chooseBadStep(
    const std::vector<PotentialStep>& potentialSteps) {

    auto changedPotentialSteps = potentialSteps;

    for (PotentialStep& step : changedPotentialSteps) {
        processStep(step);
    }
    return getDelegatedChooser().chooseBadStep(changedPotentialSteps);
}

Step MonitorDefendingChooser::chooseGoodStep(
    const std::vector<PotentialStep>& potentialSteps) {

    auto changedPotentialSteps = potentialSteps;

    for (PotentialStep& step : changedPotentialSteps) {
        processStep(step);
    }

    return getDelegatedChooser().chooseGoodStep(changedPotentialSteps);
}

void MonitorDefendingChooser::processStep(PotentialStep& step) {

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
        double opponentWeight = 0.0;
        auto nextSteps = calculatePotentialSteps(
            newGameState, opponentsInfo, opponentId, opponentExtraField);
        for (const PotentialStep& nextStep : nextSteps) {
            const auto& reachablePoints =
                nextStep.targetTrack->getReachablePoints(
                    nextStep.targetTrack->getPrincess(opponentId));

            // TODO work only with the target monitor of the opponent
            int reachableMonitors = 0;
            for (Point p : reachablePoints) {
                if (nextStep.targetTrack->getField(p).monitor != -1) {
                    ++reachableMonitors;
                }
            }
            opponentWeight += reachableMonitors / gi.numDisplays;
        }
        assert(!nextSteps.empty());
        opponentWeight /= nextSteps.size();

        weight += opponentWeight;
    }
    weight /= gi.numPlayers;

    // TODO how to evaluate weights if we get steps from e.g. lookahedChooser
    step.weight += (1.0 - weight) * weightMultiplier;
}
