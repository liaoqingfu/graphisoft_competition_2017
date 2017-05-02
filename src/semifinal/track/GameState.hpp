#ifndef SEMIFINAL_TRACK_GAMESTATE_HPP
#define SEMIFINAL_TRACK_GAMESTATE_HPP

#include "Track.hpp"
#include "Transformations.hpp"

#include <ostream>
#include <string>
#include <vector>

// Static data, does not change during a game for a player
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

    // TODO We might rather have this structure
    //struct GameDesc {
        //const GameInfo gameInfo;
        //GameState gameState;
    //};
    GameInfo gameInfo; // TODO should be const

    int currentTick = -1;
    int activePlayerId = -1;
    int targetMonitor = -1;
    int extraField = -1;

    Track track;

    GameState() = default;
    explicit GameState(GameInfo gameInfo) : gameInfo(std::move(gameInfo)) {}
    //GameState(Track track) : track(std::move(track)) {}
    GameState(const GameState& other, Track track) :
        gameInfo(other.gameInfo),
        currentTick(other.currentTick),
        activePlayerId(other.activePlayerId),
        targetMonitor(other.targetMonitor),
        extraField(other.extraField),
        track(std::move(track)) {}

    bool ourTurn() { return gameInfo.playerId == activePlayerId; }
    Point ourPosition() { return track.getPrincess(gameInfo.playerId); }
};

struct Step {
    Directions pushDirection;
    int pushPosition;
    int pushFieldType;
    Point princessTarget;
};

inline
bool operator==(const Step& lhs, const Step& rhs) {
    return lhs.pushDirection == rhs.pushDirection
            && lhs.pushPosition == rhs.pushPosition
            && lhs.pushFieldType == rhs.pushFieldType
            && lhs.princessTarget == rhs.princessTarget;
}

inline bool operator!=(const Step& lhs, const Step& rhs) {
    return !(lhs == rhs);
}

inline
std::ostream& operator<<(std::ostream& os, const Step& step) {
    os << "push " << step.pushDirection << " " << step.pushPosition << " "
       << fieldTypes[step.pushFieldType] << " princess=" << step.princessTarget;
    return os;
}

GameInfo parseInitial(const std::vector<std::string>& input);
void parseTickInfo(GameState& gameState,
        const std::vector<std::string>& input);
std::vector<std::string> createOutput(const Track& track,
        const Step& step, Point ourPosition);

inline
int executeStep(Track& track, int playerId, const Step& step,
        bool checkMovability = true) {
    int result = track.moveFields(step.pushDirection, step.pushPosition,
            step.pushFieldType);
    if (!checkMovability ||
            track.canMovePrincess(playerId, step.princessTarget)) {
        track.movePrincess(playerId, step.princessTarget);
    }
    return result;
}

class TemporaryStep {
public:
    TemporaryStep(const GameState& gameState, const Step& step,
            int player = -1) :
            gameState(const_cast<GameState&>(gameState)),
            playerId(player >= 0 ? player : gameState.gameInfo.playerId) {
        stepBack.pushDirection = static_cast<Directions>(
                oppositeDirection(step.pushDirection));
        stepBack.pushPosition = step.pushPosition;
        stepBack.princessTarget = this->gameState.track.getPrincess(playerId);
        this->gameState.extraField = stepBack.pushFieldType =
                executeStep(this->gameState.track, playerId, step);
    }

    TemporaryStep(const GameState& gameState, const Step& step,
            std::vector<Point>& points, int player = -1) :
            TemporaryStep(gameState, step, player) {
        this->points = &points;
        transformPoints(gameState.track, points, step.pushDirection,
                step.pushPosition);
    }

    ~TemporaryStep() {
        gameState.extraField =
                executeStep(gameState.track, playerId, stepBack, false);
        if (points) {
        transformPoints(gameState.track, *points, stepBack.pushDirection,
                stepBack.pushPosition);
        }
    }

    TemporaryStep(const TemporaryStep&) = delete;
    TemporaryStep& operator=(const TemporaryStep&) = delete;
    TemporaryStep(TemporaryStep&&) = delete;
    TemporaryStep& operator=(TemporaryStep&&) = delete;

private:
    GameState& gameState;
    int playerId;
    Step stepBack;
    std::vector<Point>* points = nullptr;
};

#endif // SEMIFINAL_TRACK_GAMESTATE_HPP
