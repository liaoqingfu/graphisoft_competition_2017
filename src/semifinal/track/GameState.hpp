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
    int numPlayers = 4; // in semifinal this is const
    int playerId = -1;
    int activePlayerId = -1;
    int targetMonitor = -1;
    int extraField = -1;

    GameState() = default;
    GameState(Track track) : track(std::move(track)) {}
    GameState(const GameState& other, Track track) :
        track(std::move(track)),
        width(other.width),
        height(other.height),
        numDisplays(other.numDisplays),
        maxTick(other.maxTick),
        currentTick(other.currentTick),
        numPlayers(other.numPlayers),
        playerId(other.playerId),
        activePlayerId(other.activePlayerId),
        targetMonitor(other.targetMonitor),
        extraField(other.extraField) {}

    bool ourTurn() { return playerId == activePlayerId; }
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
int executeStep(Track& track, int playerId, const Step& step) {
    int result = track.moveFields(step.pushDirection, step.pushPosition,
            step.pushFieldType);
    if (track.canMovePrincess(playerId, step.princessTarget)) {
        track.movePrincess(playerId, step.princessTarget);
    }
    return result;
}

#endif // SEMIFINAL_TRACK_GAMESTATE_HPP
