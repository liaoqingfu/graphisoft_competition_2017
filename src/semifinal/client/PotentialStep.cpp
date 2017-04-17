#include "PotentialStep.hpp"

namespace {

PotentialStep createPotentialStep(const GameState& gameState, Step step) {
    Track targetTrack{gameState.track};
    int extraField = executeStep(targetTrack, gameState.playerId, step);
    return PotentialStep{&gameState, targetTrack, extraField, std::move(step)};
}

} // unnamed namespace

std::vector<PotentialStep> calculatePotentialSteps(const GameState& gameState) {
    const Track& track = gameState.track;
    auto potentialFieldTypes = getIsomorphs(gameState.extraField);
    std::vector<PotentialStep> result;
    result.reserve((track.width() + track.height())
            * potentialFieldTypes.size() * 2);
    for (int fieldType : potentialFieldTypes) {
        for (int x = 0; x < static_cast<int>(track.width()); ++x) {
            result.push_back(createPotentialStep(gameState,
                    Step{down, x, fieldType,
                            track.getPrincess(gameState.playerId)}));
            result.push_back(createPotentialStep(gameState,
                    Step{up, x, fieldType,
                            track.getPrincess(gameState.playerId)}));
        }
        for (int y = 0; y < static_cast<int>(track.height()); ++y) {
            result.push_back(createPotentialStep(gameState,
                    Step{right, y, fieldType,
                            track.getPrincess(gameState.playerId)}));
            result.push_back(createPotentialStep(gameState,
                    Step{left, y, fieldType,
                            track.getPrincess(gameState.playerId)}));
        }
    }
    return result;
}

