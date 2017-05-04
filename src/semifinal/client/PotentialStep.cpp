#include "PotentialStep.hpp"

#include <algorithm>

std::vector<PotentialStep> calculatePotentialSteps(
    const GameState& gameState, const OpponentsInfo& opponentsInfo,
    int playerId, int extraField) {

    const Track& track = gameState.track;
    auto potentialFieldTypes = getIsomorphs(extraField);
    std::vector<PotentialStep> result;
    result.reserve((track.width() + track.height())
            * potentialFieldTypes.size() * 2);

    Point root = track.getPrincess(playerId);

    auto isBlocked = [&](Directions direction, int position) {
        auto& blocked = gameState.gameInfo.blocked;
        std::vector<Point>::const_iterator it;
        if (direction == left || direction == right) {
            it = std::find_if(
                blocked.begin(), blocked.end(),
                [position](const Point& p) { return p.y == position; });
        } else {
            it = std::find_if(
                blocked.begin(), blocked.end(),
                [position](const Point& p) { return p.x == position; });
        }
        return it != blocked.end();
    };

    auto addStep = [&](Directions direction, int position, int fieldType) {
        std::array<Point, 1> target{{root}};
        transformPoints(track, target, direction, position);
        result.emplace_back(gameState, opponentsInfo,
                Step{direction, position, fieldType, target[0]}, playerId);
    };
    auto addSteps = [&](Directions direction, int position, int fieldType) {
        if (isBlocked(direction, position)) {
            return;
        }
        addStep(direction, position, fieldType);
        addStep(static_cast<Directions>(oppositeDirection(direction)), position,
                fieldType);
    };

    int maxDistance = std::max({root.x + 1, root.y + 1,
            static_cast<int>(track.width()) - root.x,
            static_cast<int>(track.height()) - root.y});

    for (int fieldType : potentialFieldTypes) {
        addSteps(down, root.x, fieldType);
        addSteps(left, root.y, fieldType);
    }
    for (int distance = 1; distance < maxDistance; ++distance) {
        for (int fieldType : potentialFieldTypes) {
            int value = root.x - distance;
            if (value >= 0) {
                addSteps(down, value, fieldType);
            }

            value = root.x + distance;
            if (value < static_cast<int>(track.width())) {
                addSteps(down, value, fieldType);
            }

            value = root.y - distance;
            if (value >= 0) {
                addSteps(left, value, fieldType);
            }

            value = root.y + distance;
            if (value < static_cast<int>(track.height())) {
                addSteps(left, value, fieldType);
            }
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
