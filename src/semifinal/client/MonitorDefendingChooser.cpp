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
    if (iterator != savedWeights.end()) {
        // std::cerr << "Saved weight: " << iterator->second << "\n";
        step.weight += iterator->second;
        return;
    }

    GameState newGameState{*step.sourceState, *step.targetTrack};
    newGameState.extraField = step.targetExtraField;
    const auto& gi = newGameState.gameInfo;

    const auto& opponentsInfo = *step.opponentsInfo;

    std::cerr << "Step " << step.step << "\n";
    double monitorWeight = 0;
    double reachabilityWeight = 0;
    double totalArea = step.targetTrack->width() * step.targetTrack->height();
    double playerNumDivisor = gi.numPlayers * (gi.numPlayers - 1) / 2;
    for (int opponentId = 0; opponentId < gi.numPlayers;
         ++opponentId) {

        // skip ourself
        if (opponentId == gi.playerId) continue;

        int opponentExtraField = opponentsInfo[opponentId].extraField;
        const auto& targets = opponentsInfo[opponentId].targetMonitors;

        auto nextSteps = calculatePotentialSteps(
            newGameState, opponentsInfo, opponentId, opponentExtraField);
        std::unordered_set<int> reachableMonitors;
        std::size_t reachability = 0;
        for (const PotentialStep& nextStep : nextSteps) {
            const auto& reachablePoints =
                nextStep.targetTrack->getReachablePoints(
                    nextStep.targetTrack->getPrincess(opponentId));
            reachability = std::max(reachability, reachablePoints.size());
            for (Point p : reachablePoints) {
                int monitor = nextStep.targetTrack->getField(p).monitor;
                if (monitor != -1 && targets.count(monitor) != 0) {
                    reachableMonitors.insert(monitor);
                }
            }
        }

        double opponentMultiplier = gi.numPlayers
                - (opponentId - gi.playerId + gi.numPlayers) % gi.numPlayers;

        double mw = static_cast<double>(reachableMonitors.size())
                / step.sourceState->track.getAliveMonitors().size();
        std::cerr << "  Player " << opponentId << ": reachable monitors = "
                << reachableMonitors.size() << " w=" << mw << "\n";
        monitorWeight += mw * opponentMultiplier;

        double rw = static_cast<double>(reachability) / totalArea;
        std::cerr << "  Player " << opponentId << ": reachable area = "
                << reachability << " w=" << rw << "\n";
        reachabilityWeight += rw * opponentMultiplier;
    }
    monitorWeight /= playerNumDivisor;
    reachabilityWeight /= playerNumDivisor;

    double w = (1.0 - monitorWeight) * monitorWeightMultiplier
            + (1.0 - reachabilityWeight) * reachabilityWeightMultiplier;
    std::cerr << "Total weight = " << w << "\n";
    step.weight += w;
    savedWeights.emplace(key, w);
}
