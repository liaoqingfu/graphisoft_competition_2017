#include "PotentialStep.hpp"

std::vector<PotentialStep> calculatePotentialSteps(
    const GameState& gameState, const OpponentsInfo& opponentsInfo,
    int playerId, int extraField) {

    const Track& track = gameState.track;
    auto potentialFieldTypes = getIsomorphs(extraField);
    std::vector<PotentialStep> result;
    result.reserve((track.width() + track.height())
            * potentialFieldTypes.size() * 2);
    for (int fieldType : potentialFieldTypes) {
        for (int x = 0; x < static_cast<int>(track.width()); ++x) {
            result.emplace_back(gameState, opponentsInfo,
                    Step{down, x, fieldType, track.getPrincess(playerId)},
                    playerId);
            result.emplace_back(gameState, opponentsInfo,
                    Step{up, x, fieldType, track.getPrincess(playerId)},
                    playerId);
        }
        for (int y = 0; y < static_cast<int>(track.height()); ++y) {
            result.emplace_back(gameState, opponentsInfo,
                    Step{right, y, fieldType, track.getPrincess(playerId)},
                    playerId);
            result.emplace_back(gameState, opponentsInfo,
                    Step{left, y, fieldType, track.getPrincess(playerId)},
                    playerId);
        }
    }
    return result;
}

std::vector<PotentialStep> calculatePotentialSteps(
    const GameState& gameState, const OpponentsInfo& opponentsInfo) {
    return calculatePotentialSteps(gameState, opponentsInfo,
                                   gameState.gameInfo.playerId,
                                   gameState.extraField);
}

