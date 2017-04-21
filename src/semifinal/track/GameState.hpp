#ifndef SEMIFINAL_TRACK_GAMESTATE_HPP
#define SEMIFINAL_TRACK_GAMESTATE_HPP

#include "Track.hpp"

#include <string>
#include <vector>

// Static data, does not change during a game
struct GameInfo {
    int width = -1;
    int height = -1;
    int numDisplays = -1;
    int maxTick = -1;
    int numPlayers = 4; // in semifinal this is const
    int playerId = -1;
};

// Dynamic data, might change with every tick
struct GameState {

    const GameInfo gameInfo;

    int currentTick = -1;
    int activePlayerId = -1;
    int targetMonitor = -1;
    int extraField = -1;

    Track track;

    GameState() = default;
    GameState(GameInfo gameInfo) : gameInfo(std::move(gameInfo)) {}
    //GameState(Track track) : track(std::move(track)) {}
    GameState(const GameState& other, Track track) :
        gameInfo(other.gameInfo),
        currentTick(other.currentTick),
        activePlayerId(other.activePlayerId),
        targetMonitor(other.targetMonitor),
        extraField(other.extraField),
        track(std::move(track)) {}

    bool ourTurn() { return gameInfo.playerId == activePlayerId; }
};

//struct GameDesc {
    //GameInfo gameInfo;
    //GameState gameState;
//};

struct Step {
    Directions pushDirection;
    int pushPosition;
    int pushFieldType;
    Point princessTarget;
};

GameInfo parseInitial(const std::vector<std::string>& input);
void parseTickInfo(GameState& gameState,
        const std::vector<std::string>& input);
std::vector<std::string> createOutput(const Step& step);

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
