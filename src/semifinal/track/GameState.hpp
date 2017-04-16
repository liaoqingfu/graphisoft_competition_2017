#ifndef SEMIFINAL_TRACK_GAMESTATE_HPP
#define SEMIFINAL_TRACK_GAMESTATE_HPP

#include "Track.hpp"

#include <string>
#include <vector>

struct GameState {
    Track track;
    int width = -1;
    int height = -1;
    int numDisplays = -1;
    int maxTick = -1;
    int currentTick = -1;
    int playerId = -1;
    int targetMonitor = -1;
    int extraField = -1;
};

struct Step {
    Directions pushDirection;
    int pushPosition;
    int pushFieldType;
    Point princessTarget;
};

GameState parseInitialGameState(const std::vector<std::string>& input); // TODO
void parseTickInfo(GameState& gameState,
        const std::vector<std::string>& input); // TODO
std::vector<std::string> createOutput(const Step& step); // TODO

inline
void executeStep(Track& track, int playerId, const Step& step) {
    track.moveFields(step.pushDirection, step.pushPosition, step.pushFieldType);
    track.movePrincess(playerId, step.princessTarget);
}

#endif // SEMIFINAL_TRACK_GAMESTATE_HPP
