#include "PotentialStep.hpp"

std::vector<PotentialStep> calculatePotentialSteps(
    const GameState& gameState, const OpponentsInfo& opponentsInfo,
    int playerId, int extraField) {

    const Track& track = gameState.track;
    auto potentialFieldTypes = getIsomorphs(extraField);
    std::vector<PotentialStep> result;
    result.reserve((track.width() + track.height())
            * potentialFieldTypes.size() * 2);
    Point princessTarget = track.getPrincess(playerId);
    auto addResult = [&](Directions direction, int position, int fieldType) {
        std::array<Point, 1> target{{princessTarget}};
        transformPoints(track, target, direction, position);
        result.emplace_back(gameState, opponentsInfo,
                Step{direction, position, fieldType, target[0]}, playerId);
    };
    for (int fieldType : potentialFieldTypes) {
        for (int x = 0; x < static_cast<int>(track.width()); ++x) {
            addResult(down, x, fieldType);
            addResult(up, x, fieldType);
        }
        for (int y = 0; y < static_cast<int>(track.height()); ++y) {
            addResult(right, y, fieldType);
            addResult(left, y, fieldType);
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
