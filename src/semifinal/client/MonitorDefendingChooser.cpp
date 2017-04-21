#include "MonitorDefendingChooser.hpp"
#include "ChooserHelper.hpp"

Step MonitorDefendingChooser::chooseBadStep(
    const std::vector<PotentialStep>& potentialSteps) {

    // TODO why chooseBadStep takes it's param as const& ?
    auto changedPotentialSteps = potentialSteps;

    for (PotentialStep& step : changedPotentialSteps) {
        processStep(step);
    }
    return getDelegatedChooser().chooseBadStep(changedPotentialSteps);
}

Step MonitorDefendingChooser::chooseGoodStep(
    const std::vector<PotentialStep>& potentialSteps) {

    // TODO why chooseGoodStep takes it's param as const& ?
    auto changedPotentialSteps = potentialSteps;

    for (PotentialStep& step : changedPotentialSteps) {
        processStep(step);
    }

    auto it = std::max_element(
        changedPotentialSteps.begin(), changedPotentialSteps.end(),
        [](const auto& s1, const auto& s2) { return s1.weight < s2.weight; });
    std::cerr << "MonitorDefendingChooser: step with greatest weight ("
              << it->weight << ") "
              << it->step << "\n";

    return getDelegatedChooser().chooseGoodStep(changedPotentialSteps);
}

void MonitorDefendingChooser::processStep(PotentialStep step) {

    GameState newGameState{*step.sourceState, *step.targetTrack};
    newGameState.extraField = step.targetExtraField;
    const auto& gi = newGameState.gameInfo;

    // TODO deduce from gameState what is the opponent's extraField
    constexpr int opponentExtraField = 15;

    double weight = 0;
    for (int opponentId = 0; opponentId < gi.numPlayers;
         ++opponentId) {

        // skip ourself
        if (opponentId == gi.playerId) continue;

        double opponentWeight = 0.0;
        auto nextSteps = calculatePotentialSteps(newGameState, opponentId,
                                                 opponentExtraField);
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
    step.weight *= (1.0 - weight);
}
