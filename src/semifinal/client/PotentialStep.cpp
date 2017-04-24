#include "PotentialStep.hpp"

namespace {

PotentialStep createPotentialStep(const GameState& gameState, const OpponentsInfo& opponentsInfo, Step step, int playerId) {
    auto targetTrack = std::make_shared<Track>(gameState.track);
    int extraField = executeStep(*targetTrack, playerId, step);
    return PotentialStep{&gameState, &opponentsInfo, targetTrack, extraField, std::move(step),
            1.0, {}};
}

} // unnamed namespace

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
            result.push_back(createPotentialStep(
                gameState, opponentsInfo,
                Step{down, x, fieldType, track.getPrincess(playerId)},
                playerId));
            result.push_back(createPotentialStep(
                gameState, opponentsInfo,
                Step{up, x, fieldType, track.getPrincess(playerId)}, playerId));
        }
        for (int y = 0; y < static_cast<int>(track.height()); ++y) {
            result.push_back(createPotentialStep(
                gameState, opponentsInfo,
                Step{right, y, fieldType, track.getPrincess(playerId)},
                playerId));
            result.push_back(createPotentialStep(
                gameState, opponentsInfo,
                Step{left, y, fieldType, track.getPrincess(playerId)},
                playerId));
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

