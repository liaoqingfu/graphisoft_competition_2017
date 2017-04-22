#include "ChoosingStrategy.hpp"

#include "PotentialStep.hpp"

#include <iostream>
#include <vector>

Step ChoosingStrategy::calculateStep() {
    auto potentialSteps = calculatePotentialSteps(gameState);
    std::vector<PotentialStep> goodSteps;
    std::remove_copy_if(potentialSteps.begin(), potentialSteps.end(),
            std::back_inserter(goodSteps),
            [](const PotentialStep& step) {
                return !step.targetTrack->canMovePrincess(
                        step.sourceState->gameInfo.playerId,
                        step.targetTrack->getMonitor(
                                step.sourceState->targetMonitor));
            });
    if (goodSteps.empty()) {
        return chooser->chooseBadStep(potentialSteps);
    }
    potentialSteps.clear();
    for (PotentialStep& step : goodSteps) {
        Point target = step.targetTrack->getMonitor(gameState.targetMonitor);
        step.step.princessTarget = target;
        // std::cerr << "Good step: " << step.step.pushDirection
        //         << " " << step.step.pushPosition
        //         << " " << fieldTypes[step.step.pushFieldType]
        //         << " " << step.step.princessTarget
        //         << "\n";

        assert(step.targetTrack.use_count() == 1);
        const_cast<Track&>(*step.targetTrack).movePrincess(
                gameState.gameInfo.playerId, target);
    }
    return chooser->chooseGoodStep(goodSteps);
}

