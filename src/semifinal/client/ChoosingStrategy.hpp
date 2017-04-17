#ifndef SEMIFINAL_CLIENT_CHOOSINGSTRATEGY_HPP
#define SEMIFINAL_CLIENT_CHOOSINGSTRATEGY_HPP

#include "GameState.hpp"

#include <iostream>
#include <string>
#include <vector>

struct PotentialStep {
    const GameState* sourceState;
    Track targetTrack;
    Step step;
};

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
        auto potentialSteps = calculatePotentialSteps();
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

    std::vector<PotentialStep> calculatePotentialSteps() {
        const Track& track = gameState.track;
        auto potentialFieldTypes = getIsomorphs(gameState.extraField);
        std::vector<PotentialStep> result;
        result.reserve((track.width() + track.height())
                * potentialFieldTypes.size() * 2);
        for (int fieldType : potentialFieldTypes) {
            for (int x = 0; x < static_cast<int>(track.width()); ++x) {
                result.push_back(createPotentialStep(
                        Step{down, x, fieldType,
                                track.getPrincess(gameState.playerId)}));
                result.push_back(createPotentialStep(
                        Step{up, x, fieldType,
                                track.getPrincess(gameState.playerId)}));
            }
            for (int y = 0; y < static_cast<int>(track.height()); ++y) {
                result.push_back(createPotentialStep(
                        Step{right, y, fieldType,
                                track.getPrincess(gameState.playerId)}));
                result.push_back(createPotentialStep(
                        Step{left, y, fieldType,
                                track.getPrincess(gameState.playerId)}));
            }
        }
        return result;
    }

    PotentialStep createPotentialStep(Step step) {
        Track targetTrack{gameState.track};
        executeStep(targetTrack, gameState.playerId, step);
        return PotentialStep{&gameState, targetTrack, std::move(step)};
    }

    Chooser chooser;
    GameState gameState;
};

#endif // SEMIFINAL_CLIENT_CHOOSINGSTRATEGY_HPP
