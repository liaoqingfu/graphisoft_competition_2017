#ifndef SEMIFINAL_CLIENT_CHOOSINGSTRATEGY_HPP
#define SEMIFINAL_CLIENT_CHOOSINGSTRATEGY_HPP

#include "IChooser.hpp"
#include "GameState.hpp"
#include "PotentialStep.hpp"

#include <memory>
#include <iostream>
#include <vector>

class ChoosingStrategy {
public:
    ChoosingStrategy() = default;

    ChoosingStrategy(std::shared_ptr<IChooser> chooser) :
            chooser(std::move(chooser)) {}

    ChoosingStrategy(const ChoosingStrategy&) = default;
    ChoosingStrategy& operator=(const ChoosingStrategy&) = default;

    ChoosingStrategy(ChoosingStrategy&&) noexcept = default;
    ChoosingStrategy& operator=(ChoosingStrategy&&) = default;

    Step ourTurn(GameState gameState) {
        this->gameState = std::move(gameState);
        return calculateStep();
    }

    void opponentsTurn(const Track& /*track*/, int /*playerId*/) {
        // TODO implement
    }

private:
    Step calculateStep() {
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

    std::shared_ptr<IChooser> chooser;
    GameState gameState;
};

#endif // SEMIFINAL_CLIENT_CHOOSINGSTRATEGY_HPP
