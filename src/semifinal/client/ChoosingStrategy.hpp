#ifndef SEMIFINAL_CLIENT_CHOOSINGSTRATEGY_HPP
#define SEMIFINAL_CLIENT_CHOOSINGSTRATEGY_HPP

#include "GameState.hpp"
#include "PotentialStep.hpp"

#include <iostream>
#include <vector>

template<typename Chooser>
class ChoosingStrategy {
public:
    ChoosingStrategy(Chooser chooser) : chooser(chooser) {}

    Step operator()(GameState gameState) {
        this->gameState = std::move(gameState);
        return calculateStep();
    }

private:
    Step calculateStep() {
        auto potentialSteps = calculatePotentialSteps(gameState);
        std::vector<PotentialStep> goodSteps;
        std::remove_copy_if(potentialSteps.begin(), potentialSteps.end(),
                std::back_inserter(goodSteps),
                [](const PotentialStep& step) {
                    return !step.targetTrack.canMovePrincess(
                            step.sourceState->playerId,
                            step.targetTrack.getMonitor(
                                    step.sourceState->targetMonitor));
                });
        if (goodSteps.empty()) {
            return chooser.chooseBadStep(potentialSteps);
        }
        for (PotentialStep& step : goodSteps) {
            Point target = step.targetTrack.getMonitor(gameState.targetMonitor);
            step.step.princessTarget = target;
            // std::cerr << "Good step: " << step.step.pushDirection
            //         << " " << step.step.pushPosition
            //         << " " << fieldTypes[step.step.pushFieldType]
            //         << " " << step.step.princessTarget
            //         << "\n";
            step.targetTrack.movePrincess(gameState.playerId, target);
        }
        return chooser.chooseGoodStep(goodSteps);
    }

    Chooser chooser;
    GameState gameState;
};

#endif // SEMIFINAL_CLIENT_CHOOSINGSTRATEGY_HPP
