#include "ChoosingStrategy.hpp"

#include "PotentialStep.hpp"

#include <iostream>
#include <vector>

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

Point getRemovedMonitor(const Track::Monitors& prev, const Track::Monitors& current) {
    assert (prev.size() == current.size());
    for (unsigned i = 0; i < prev.size(); ++i) {
        if (prev[i] != current[i]) {
            return prev[i];
        }
    }
    return -p11;
}

void ChoosingStrategy::setTargetMonitors(const Track& currentTrack) {

    auto& targetMonitors = opponentsInfo[prevSt.playerId].targetMonitors;

    const auto& prevTrack = this->prevSt.gameState.track;
    auto prevPlayerId = prevSt.playerId;

    auto monitorDiff = prevTrack.getRemainingMonitors() -
                           currentTrack.getRemainingMonitors();
    assert(monitorDiff == 0 || monitorDiff == 1);

    auto remove = [](auto& targetMonitors, auto p) {
        targetMonitors.erase(
            std::remove(targetMonitors.begin(), targetMonitors.end(), p),
            targetMonitors.end());
    };

    // one monitor removed
    if (prevTrack.getRemainingMonitors() !=
        currentTrack.getRemainingMonitors()) {

        // reset to all monitors which are still on the track
        targetMonitors = currentTrack.getAliveMonitors();

        // remove the monitor from all others targets
        for (int i = 0; i < (int)opponentsInfo.size(); ++i) {
            if (i == prevSt.playerId) continue;
            auto p = getRemovedMonitor(prevTrack.getAllMonitors(),
                                       currentTrack.getAllMonitors());
            remove(opponentsInfo[i].targetMonitors, p);
        }
        return;
    }

    auto potentialSteps = calculatePotentialSteps(
        prevSt.gameState, opponentsInfo, prevPlayerId,
        opponentsInfo[prevSt.playerId].extraField);

    for (const PotentialStep& step : potentialSteps) {
        const auto& reachablePoints =
            step.targetTrack->getReachablePoints(
                step.targetTrack->getPrincess(prevPlayerId));

        // find a monitor which was reachable, but the opponent did not step
        // onto that
        for (Point p : reachablePoints) {
            int monitor = step.targetTrack->getField(p).monitor;
            if (monitor != -1) {
                remove(targetMonitors, p);
            }
        }
    }
}

void ChoosingStrategy::updateOpponentsInfo(const Track& track, int playerId) {

    if (gameState.currentTick == 0) {
        for (auto& oi : opponentsInfo) {
            oi.targetMonitors = gameState.track.getAliveMonitors();
        }
    }

    if (prevSt.playerId != -1 &&
        prevSt.playerId != gameState.gameInfo.playerId) {

        assert(playerId >= 0 && playerId < gameState.gameInfo.numPlayers);
        assert(prevSt.playerId >= 0 &&
               prevSt.playerId < gameState.gameInfo.numPlayers);

        opponentsInfo[prevSt.playerId].extraField =
            getExtraField(prevSt.gameState.track, track);

        setTargetMonitors(track);
    }
    prevSt = {GameState(this->gameState, track), playerId};
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
