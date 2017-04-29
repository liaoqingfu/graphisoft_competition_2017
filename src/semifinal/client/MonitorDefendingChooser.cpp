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
    auto key = std::make_tuple(step.getStep().pushDirection,
            step.getStep().pushPosition, step.getStep().pushFieldType);
    auto iterator = savedWeights.find(key);
    if (iterator != savedWeights.end()) {
        // std::cerr << "Saved weight: " << iterator->second << "\n";
        step.addWeight(iterator->second);
        step.addDebugInfo(PotentialStep::DebugInfo{
                "MonitorDefendingChooser:total", 0, iterator->second});
        return;
    }

    GameState newGameState = step.createNewGameState();
    const auto& gi = newGameState.gameInfo;

    const OpponentsInfo& opponentsInfo = step.getOpponentInfo();

    // std::cerr << "Step " << step.step << "\n";
    double monitorWeight = 0;
    double reachabilityWeight = 0;
    const Track& targetTrack = step.getTargetTrack();
    double totalArea = targetTrack.width() * targetTrack.height();
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
            const Track& nextTargetTrack = nextStep.getTargetTrack();
            const auto& reachablePoints =
                nextTargetTrack.getReachablePoints(
                    nextTargetTrack.getPrincess(opponentId));
            reachability = std::max(reachability, reachablePoints.size());
            for (Point p : reachablePoints) {
                int monitor = nextTargetTrack.getField(p).monitor;
                if (monitor != -1 && targets.count(monitor) != 0) {
                    reachableMonitors.insert(monitor);
                }
            }
        }

        double opponentMultiplier = gi.numPlayers
                - (opponentId - gi.playerId + gi.numPlayers) % gi.numPlayers;

        const GameState& sourceState = step.getSourceState();
        double mw = static_cast<double>(reachableMonitors.size())
                / sourceState.track.getAliveMonitors().size();

        const int& ourTargetMonitor = sourceState.targetMonitor;
        // In this step we are not going to catch out target Monitor ...
        if (step.getStep().princessTarget !=
                targetTrack.getMonitor(ourTargetMonitor)
            // ... but this potential step would allow the opponent to reach it
            &&
            targets.count(ourTargetMonitor) > 0) {

            mw *= 2;
        }

        // std::cerr << "  Player " << opponentId << ": reachable monitors = "
        //         << reachableMonitors.size() << " w=" << mw << "\n";
        monitorWeight += mw * opponentMultiplier;

        double rw = static_cast<double>(reachability) / totalArea;
        // std::cerr << "  Player " << opponentId << ": reachable area = "
        //         << reachability << " w=" << rw << "\n";
        reachabilityWeight += rw * opponentMultiplier;
    }
    monitorWeight /= playerNumDivisor;
    reachabilityWeight /= playerNumDivisor;

    double mw = (1.0 - monitorWeight);
    double mww = mw * monitorWeightMultiplier;
    double rw = (1.0 - reachabilityWeight);
    double rww = rw * reachabilityWeightMultiplier;
    step.addDebugInfo(PotentialStep::DebugInfo{"MonitorDefendingChooser:monitors", mw, mww});
    step.addDebugInfo(PotentialStep::DebugInfo{"MonitorDefendingChooser:reachability", rw, rww});
    step.addWeight(mww + rww);
    savedWeights.emplace(key, mww + rww);
}
