#include "ChoosingStrategy.hpp"

#include "Hash.hpp"
#include "PotentialStep.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

Step ChoosingStrategy::calculateStep() {
    auto potentialSteps = calculatePotentialSteps(gameState, getOpponentsInfo());
    std::vector<PotentialStep> goodSteps;
    std::remove_copy_if(potentialSteps.begin(), potentialSteps.end(),
            std::back_inserter(goodSteps),
            [](const PotentialStep& step) {
                TemporaryStep temporaryStep{step.getGameState(),
                        step.getStep()};
                const Track& track = step.getGameState().track;
                const GameState& gameState = step.getGameState();
                return !track.canMovePrincess(
                        gameState.gameInfo.playerId,
                        track.getMonitor(gameState.targetMonitor));
            });
    if (goodSteps.empty()) {
        return chooser->chooseBadStep(potentialSteps);
    }
    potentialSteps.clear();
    for (PotentialStep& step : goodSteps) {
        TemporaryStep temporaryStep{step.getGameState(),
                step.getStep()};
        const Track& track = step.getGameState().track;
        Point target = track.getMonitor(gameState.targetMonitor);
        step.setPrincessTarget(target);
        // std::cerr << "Good step: " << step.step.pushDirection
        //         << " " << step.step.pushPosition
        //         << " " << fieldTypes[step.step.pushFieldType]
        //         << " " << step.step.princessTarget
        //         << "\n";
    }
    return chooser->chooseGoodStep(goodSteps);
}

namespace {

constexpr int defaultExtraField = 15;

Push getPush(const Track& prevTrack, const Track& currentTrack,
        std::size_t opponentId, std::size_t numPlayers) {
    // Check if a monitor or a player other than the opponent is moved.
    std::size_t numMonitors = prevTrack.getAllMonitors().size();
    for (std::size_t i = 0; i < numMonitors; ++i) {
        Point currentMonitor = currentTrack.getMonitor(i);
        if (currentMonitor.x < 0) {
            continue;
        }
        for (std::size_t neighbor = 0; neighbor < numNeighbors; ++neighbor) {
            if (currentMonitor == prevTrack.getMonitor(i) +
                    neighbors[neighbor]) {
                return Push{neighbor, neighbors[neighbor].x == 0
                        ? currentMonitor.x : currentMonitor.y};
            }
        }
    }
    for (std::size_t i = 0; i < numPlayers; ++i) {
        if (i == opponentId) {
            continue;
        }
        Point currentPlayer = currentTrack.getPrincess(i);
        for (std::size_t neighbor = 0; neighbor < numNeighbors; ++neighbor) {
            if (currentPlayer == prevTrack.getPrincess(i) +
                    neighbors[neighbor]) {
                return Push{neighbor, neighbors[neighbor].x == 0
                        ? currentPlayer.x : currentPlayer.y};
            }
        }
    }

    // No monitor or player is moved. Try to check the field types.
    std::unordered_map<Push, int> evidence;
    for (Point p : matrixRange(currentTrack)) {
        int prevField = prevTrack.getField(p).type;
        if (prevField == currentTrack.getField(p).type) {
            continue;
        }
        for (std::size_t neighbor = 0; neighbor < numNeighbors; ++neighbor) {
            Point neighborPoint = p + neighbors[neighbor];
            if (!isInsideMatrix(currentTrack, neighborPoint)) {
                continue;
            }
            if (currentTrack.getField(neighborPoint).type == prevField) {
                ++evidence[Push{neighbor, neighbors[neighbor].x == 0
                        ? p.x : p.y}];
            }
        }
    }
    if (evidence.empty()) {
        return invalidPush;
    }
    const auto& best = *std::max_element(evidence.begin(), evidence.end(),
            [](const auto& lhs, const auto& rhs) {
                return lhs.second < rhs.second;
            });
    if (best.second == 0) {
        return invalidPush;
    }
    return best.first;
}

int getExtraField(const Track& prevTrack, const Push& push) {
    if (!isValid(push)) {
        return defaultExtraField;
    }
    Point p{push.position, push.position};
    switch (push.direction) {
    case left: p.x = 0; break;
    case up: p.y = 0; break;
    case right: p.x = prevTrack.width() - 1; break;
    case down: p.y = prevTrack.height() - 1; break;
    };
    return prevTrack.getField(p).type;
}

int getRemovedMonitor(const Track& prev, const Track& current) {
    const auto& prevMons = prev.getAllMonitors();
    const auto& currentMons = current.getAllMonitors();
    assert (prevMons.size() == currentMons.size());
    for (unsigned i = 0; i < prevMons.size(); ++i) {
        if (prevMons[i].x >= 0 && currentMons[i].x < 0) {
            return prev.getField(prevMons[i]).monitor;
        }
    }
    return -1;
}

} // unnamed namespace

void ChoosingStrategy::setTargetMonitors(const Track& currentTrack) {

    auto& targetMonitors = opponentsInfo[prevSt.playerId].targetMonitors;

    const auto& prevTrack = this->prevSt.gameState.track;
    auto prevPlayerId = prevSt.playerId;

    auto monitorDiff = prevTrack.getRemainingMonitors() -
                           currentTrack.getRemainingMonitors();
    if (!(monitorDiff == 0 || monitorDiff == 1)) {
        std::cerr << "ERROR!\n" << prevTrack << currentTrack;
        assert(0);
    }

    //std::cerr << "YYY player" << gameState.gameInfo.playerId << " opp"
                //<< prevSt.playerId << targetMonitors << "\n";

    auto remove = [&](auto& targetMonitors, int monitorId, int oppId) {
        (void)oppId;
        //std::cerr << "remove from targets: player"
                  //<< gameState.gameInfo.playerId << " opp" << oppId << " "
                  //<< monitorId << "\n";
        targetMonitors.erase(monitorId);
    };

    // one monitor removed
    if (prevTrack.getRemainingMonitors() !=
        currentTrack.getRemainingMonitors()) {

        // reset to monitors which are still on the track
        auto reset = [&currentTrack](auto& targets) {
            targets = currentTrack.getAliveMonitors();
        };

        reset(targetMonitors);

        // remove the monitor from all others targets
        int monitorId = getRemovedMonitor(prevTrack, currentTrack);
        // std::cerr << "One monitor Removed " << monitorId << std::endl;
        for (int i = 0; i < gameState.gameInfo.numPlayers; ++i) {
            if (i == prevSt.playerId) continue;

            if (opponentsInfo[i].targetMonitors.count(monitorId)) {
                reset(opponentsInfo[i].targetMonitors);
            } else {
                remove(opponentsInfo[i].targetMonitors, monitorId, i);
            }
        }
        // std::cerr << "XXX player" << gameState.gameInfo.playerId << " opp"
        //<< prevSt.playerId << targetMonitors << "\n";
        return;
    }

    auto potentialSteps = calculatePotentialSteps(
        prevSt.gameState, opponentsInfo, prevPlayerId,
        opponentsInfo[prevSt.playerId].extraField);

    std::unordered_set<int> toBeRemoved;
    for (const PotentialStep& step : potentialSteps) {
        TemporaryStep temporaryStep{step.getGameState(),
                step.getStep(), prevSt.playerId};
        const Track& track = step.getGameState().track;
        const auto& reachablePoints =
            track.getReachablePoints(
                track.getPrincess(prevPlayerId));

        // find a monitor which was reachable, but the opponent did not step
        // onto that
        for (Point p : reachablePoints) {
            int monitor = track.getField(p).monitor;
            if (monitor != -1) {
                toBeRemoved.insert(track.getField(p).monitor);
            }
        }
    }
    for (int i : toBeRemoved) {
        remove(targetMonitors, i, prevSt.playerId);
    }

    //std::cerr << "XXX player" << gameState.gameInfo.playerId << " opp" << prevSt.playerId << targetMonitors << "\n";
}

void ChoosingStrategy::updateOpponentsInfo(const Track& track, int opponentId) {

    std::cerr << "Opponent " << opponentId << " numPlayers " <<
            gameState.gameInfo.numPlayers << "\n";

    if (opponentsInfo[0].targetMonitors.empty()) {
        //std::cerr << "INITIALIZE TARGETS" << std::endl;
        for (auto& oi : opponentsInfo) {
            oi.targetMonitors = track.getAliveMonitors();
        }
    }

    if (prevSt.playerId != -1 ) {
        //prevSt.playerId != gameState.gameInfo.playerId) {

        assert(opponentId >= 0);
        assert(opponentId < gameState.gameInfo.numPlayers);
        assert(prevSt.playerId >= 0);
        assert(prevSt.playerId < gameState.gameInfo.numPlayers);

        const auto& prevTrack = this->prevSt.gameState.track;
        OpponentData& opponentData = opponentsInfo[prevSt.playerId];
        opponentData.lastPush = getPush(prevTrack, track, opponentId,
                gameState.gameInfo.numPlayers);
        opponentData.extraField =
            getExtraField(prevSt.gameState.track, opponentData.lastPush);
        // If the opponent just picked up a monitor, then the last push is
        // irrelevant for any further movement of this opponent.
        if (prevTrack.getRemainingMonitors() != track.getRemainingMonitors()) {
            opponentData.lastPush = invalidPush;
        }

        setTargetMonitors(track);
    }
    prevSt = {GameState(this->gameState, track), opponentId};
}

void ChoosingStrategy::opponentsTurn(const Track& track, int playerId) {
    updateOpponentsInfo(track, playerId);
}

void ChoosingStrategy::setGameState(GameState gameState) {
    this->gameState = std::move(gameState);
}

Step ChoosingStrategy::ourTurn(GameState gameState) {
    auto now = boost::posix_time::microsec_clock::universal_time();
    this->gameState = std::move(gameState);
    updateOpponentsInfo(this->gameState.track,
                        this->gameState.gameInfo.playerId);
    Step step = calculateStep();
    std::cerr << "time spent on tick: " << boost::posix_time::to_simple_string(
            boost::posix_time::microsec_clock::universal_time() - now) << "\n";
    return step;
}

void ChoosingStrategy::reset() {
    gameState = GameState{};
    opponentsInfo = OpponentsInfo{};
    prevSt = PrevState{};
}
