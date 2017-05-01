#include "OpponentPushCheckingChooser.hpp"
#include "ChooserHelper.hpp"

#include <unordered_set>

Step OpponentPushCheckingChooser::chooseBadStep(
        const std::vector<PotentialStep>& potentialSteps) {

    auto changedPotentialSteps = potentialSteps;

    for (PotentialStep& step : changedPotentialSteps) {
        processStep(step);
    }
    return getDelegatedChooser().chooseBadStep(changedPotentialSteps);
}

Step OpponentPushCheckingChooser::chooseGoodStep(
        const std::vector<PotentialStep>& potentialSteps) {

    auto changedPotentialSteps = potentialSteps;

    for (PotentialStep& step : changedPotentialSteps) {
        processStep(step);
    }

    return getDelegatedChooser().chooseGoodStep(changedPotentialSteps);
}

void OpponentPushCheckingChooser::processStep(PotentialStep& step) {
    TemporaryStep temporaryStep{step.getGameState(), step.getStep()};
    const Track& track = step.getGameState().track;
    if (step.getStep().princessTarget == track.getMonitor(
            step.getGameState().targetMonitor)) {
        return;
    }
    for (const OpponentData& opponentData : step.getOpponentInfo()) {
        Push push = opponentData.lastPush;
        if (isValid(push) && step.getStep().pushPosition == push.position
                && (step.getStep().pushDirection == push.direction
                        || step.getStep().pushDirection
                                == oppositeDirection(push.direction))) {
            step.addWeight(-weightMultiplier);
            step.addDebugInfo(PotentialStep::DebugInfo{
                    "MaxReachableChooser", -1.0, -weightMultiplier});
        }
    }
}
