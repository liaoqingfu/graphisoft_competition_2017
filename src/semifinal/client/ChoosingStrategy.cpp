#include "ChoosingStrategy.hpp"

#include "PotentialStep.hpp"

#include <iostream>
#include <vector>
#include <unordered_set>

Step ChoosingStrategy::calculateStep() {
    auto potentialSteps = calculatePotentialSteps(gameState, getOpponentsInfo());
    std::vector<PotentialStep> goodSteps;
    std::remove_copy_if(potentialSteps.begin(), potentialSteps.end(),
            std::back_inserter(goodSteps),
            [](const PotentialStep& step) {
                return !step.targetTrack->canMovePrincess(
                        step.sourceState->gameInfo.playerId,
                        step.targetTrack->getMonitor(
                                step.sourceState->targetMonitor));
            });
    if (goodSteps.empty()) {
        return chooser->chooseBadStep(potentialSteps);
    }
    potentialSteps.clear();
    for (PotentialStep& step : goodSteps) {
        Point target = step.targetTrack->getMonitor(gameState.targetMonitor);
        step.step.princessTarget = target;
        // std::cerr << "Good step: " << step.step.pushDirection
        //         << " " << step.step.pushPosition
        //         << " " << fieldTypes[step.step.pushFieldType]
        //         << " " << step.step.princessTarget
        //         << "\n";

        assert(step.targetTrack.use_count() == 1);
        const_cast<Track&>(*step.targetTrack).movePrincess(
                gameState.gameInfo.playerId, target);
    }
    return chooser->chooseGoodStep(goodSteps);
}

enum class ColRow { Column, Row };
constexpr int defaultExtraField = 15;

auto getExtraFieldFromColRow(const Track& prevTrack, const Track& currentTrack,
                             ColRow colRow, int position) {
    if (colRow == ColRow::Column) {
        for (int i = 1; i < (int)currentTrack.height(); ++i) {
            Point p1{position, i};
            Point p2{position, i - 1};
            if (prevTrack.getField(p2).type != currentTrack.getField(p1).type) {
                Point extra{position, 0};
                return prevTrack.getField(extra).type;
            }
        }
        Point extra{position, (int)currentTrack.height() - 1};
        return prevTrack.getField(extra).type;

    } else { // Row
        for (int i = 1; i < (int)currentTrack.width(); ++i) {
            Point p1{i, position};
            Point p2{i - 1, position};
            if (prevTrack.getField(p2).type != currentTrack.getField(p1).type) {
                Point extra{0, position};
                return prevTrack.getField(extra).type;
            }
        }
        Point extra{(int)currentTrack.width() - 1, position};
        return prevTrack.getField(extra).type;
    }
    return defaultExtraField;
}

auto getExtraField(const Track& prevTrack, const Track& currentTrack) {
    assert(prevTrack.width() == currentTrack.width());
    assert(prevTrack.height() == currentTrack.height());

    // go through the first row
    for (int i = 0; i < (int)currentTrack.width(); ++i) {
        Point p = {i, 0};

        // one column possible
        if (currentTrack.getField(p).type != prevTrack.getField(p).type) {
            unsigned diff = 0;
            // go through the column
            for (int j = 1; j < (int)currentTrack.height(); ++j) {
                if (prevTrack.getField({i,j}).type != currentTrack.getField({i,j}).type) ++diff;
            }

            // very possibly this column
            if (diff >= currentTrack.height() / 2) {
                return getExtraFieldFromColRow(prevTrack, currentTrack, ColRow::Column, i);
            }
        }
    }

    // go through the first column
    for (int i = 0; i < (int)currentTrack.height(); ++i) {
        Point p = {0, i};

        // one row possible
        if (currentTrack.getField(p).type != prevTrack.getField(p).type) {
            unsigned diff = 0;
            // go through the row
            for (int j = 1; j < (int)currentTrack.width(); ++j) {
                if (prevTrack.getField({j,i}).type != currentTrack.getField({j,i}).type) ++diff;
            }

            // very possibly this row
            if (diff >= currentTrack.width() / 2) {
                return getExtraFieldFromColRow(prevTrack, currentTrack, ColRow::Row, i);
            }
        }
    }

    return defaultExtraField;
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

std::ostream& operator<<(std::ostream& os, const std::set<int>& vi) {
    os << "{";
    for (auto i : vi) {
        os << i << " ";
    }
    os << "}";
    return os;
}

void ChoosingStrategy::setTargetMonitors(const Track& currentTrack) {

    auto& targetMonitors = opponentsInfo[prevSt.playerId].targetMonitors;

    const auto& prevTrack = this->prevSt.gameState.track;
    auto prevPlayerId = prevSt.playerId;

    auto monitorDiff = prevTrack.getRemainingMonitors() -
                           currentTrack.getRemainingMonitors();
    assert(monitorDiff == 0 || monitorDiff == 1);

    std::cerr << "YYY player" << gameState.gameInfo.playerId << " opp"
                << prevSt.playerId << targetMonitors << "\n";

    auto remove = [&](auto& targetMonitors, int monitorId, int oppId) {
        std::cerr << "remove from targets: player"
                  << gameState.gameInfo.playerId << " opp" << oppId << " "
                  << monitorId << "\n";
        targetMonitors.erase(monitorId);
    };

    // one monitor removed
    if (prevTrack.getRemainingMonitors() !=
        currentTrack.getRemainingMonitors()) {

        // reset to monitors which are still on the track
        targetMonitors = currentTrack.getAliveMonitors();

        // remove the monitor from all others targets
        int monitorId = getRemovedMonitor(prevTrack, currentTrack);
        std::cerr << "One monitor Removed " << monitorId << std::endl;
        for (int i = 0; i < (int)opponentsInfo.size(); ++i) {
            if (i == prevSt.playerId) continue;
            remove(opponentsInfo[i].targetMonitors, monitorId, i);
        }
        std::cerr << "XXX player" << gameState.gameInfo.playerId << " opp"
                  << prevSt.playerId << targetMonitors << "\n";
        return;
    }

    auto potentialSteps = calculatePotentialSteps(
        prevSt.gameState, opponentsInfo, prevPlayerId,
        opponentsInfo[prevSt.playerId].extraField);

    std::unordered_set<int> toBeRemoved;
    for (const PotentialStep& step : potentialSteps) {
        const auto& reachablePoints =
            step.targetTrack->getReachablePoints(
                step.targetTrack->getPrincess(prevPlayerId));

        // find a monitor which was reachable, but the opponent did not step
        // onto that
        for (Point p : reachablePoints) {
            int monitor = step.targetTrack->getField(p).monitor;
            if (monitor != -1) {
                toBeRemoved.insert(step.targetTrack->getField(p).monitor);
                //remove(targetMonitors, step.targetTrack->getField(p).monitor, prevSt.playerId);
            }
        }
    }
    for (int i : toBeRemoved) {
        remove(targetMonitors, i, prevSt.playerId);
    }

    std::cerr << "XXX player" << gameState.gameInfo.playerId << " opp" << prevSt.playerId << targetMonitors << "\n";
}

void ChoosingStrategy::updateOpponentsInfo(const Track& track, int opponentId) {

    if (opponentsInfo[0].targetMonitors.empty()) {
        std::cerr << "INITIALIZE TARGETS" << std::endl;
        for (auto& oi : opponentsInfo) {
            oi.targetMonitors = track.getAliveMonitors();
        }
    }

    if (prevSt.playerId != -1 ) {
        //prevSt.playerId != gameState.gameInfo.playerId) {

        assert(opponentId >= 0 && opponentId < gameState.gameInfo.numPlayers);
        assert(prevSt.playerId >= 0 &&
               prevSt.playerId < gameState.gameInfo.numPlayers);

        opponentsInfo[prevSt.playerId].extraField =
            getExtraField(prevSt.gameState.track, track);

        setTargetMonitors(track);
    }
    prevSt = {GameState(this->gameState, track), opponentId};
}

void ChoosingStrategy::opponentsTurn(const Track& track, int playerId) {
    updateOpponentsInfo(track, playerId);
}

Step ChoosingStrategy::ourTurn(GameState gameState) {
    this->gameState = std::move(gameState);
    updateOpponentsInfo(this->gameState.track,
                        this->gameState.gameInfo.playerId);
    return calculateStep();
}
