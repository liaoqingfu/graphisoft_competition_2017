#ifndef SEMIFINAL_TRACK_GAMESTATE_HPP
#define SEMIFINAL_TRACK_GAMESTATE_HPP

#include "Track.hpp"

#include <string>
#include <vector>

struct GameState {
    Track track;
    int maxTick;
    int currentTick;
    int playerId;
    int targetMonitor;
    int extraField;
};

GameState parseInitialGameState(const std::vector<std::string>& input); // TODO
void parseTickInfo(GameState& gameState,
        const std::vector<std::string>& input); // TODO
std::vector<std::string> createOutput(
        Direction pushDirection, int pushPosition, int pushFieldType,
        Point princessTarget); // TODO

#endif // SEMIFINAL_TRACK_GAMESTATE_HPP
