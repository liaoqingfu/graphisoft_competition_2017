#ifndef SEMIFINAL_CLIENT_HEURISTICSOLVER_HPP
#define SEMIFINAL_CLIENT_HEURISTICSOLVER_HPP

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
class HeuristicSolver {
public:
    HeuristicSolver(Chooser chooser) : chooser(std::move(chooser)) {}

    void init(const std::vector<std::string>& fieldInfos) {
        gameState = parseInitialGameState(fieldInfos);
    }

    std::vector<std::string> process(
            const std::vector<std::string>& tickInfos) {
        parseTickInfo(gameState, tickInfos);
        return createOutput(calculateStep());
    }

    void end(const std::string& message) {
        std::cerr << "Game over: " << message << std::endl;
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
            Point target = gameState.track.getMonitor(gameState.targetMonitor);
            step.step.princessTarget = target;
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

    GameState gameState;
    Chooser chooser;
};



#endif // SEMIFINAL_CLIENT_HEURISTICSOLVER_HPP
