#include "ChoosingStrategy.hpp"

#include "PotentialStep.hpp"

#include <iostream>
#include <vector>

Step ChoosingStrategy::calculateStep() {
    auto potentialSteps = calculatePotentialSteps(gameState);
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

void ChoosingStrategy::updateOpponentsInfo(const Track& track, int playerId) {
    if (prevSt.playerId != -1 &&
        prevSt.playerId != gameState.gameInfo.playerId) {

        assert(playerId >= 0 && playerId < gameState.gameInfo.numPlayers);
        assert(prevSt.playerId >= 0 &&
               prevSt.playerId < gameState.gameInfo.numPlayers);
        opponentsInfo[prevSt.playerId].extraField =
            getExtraField(prevSt.track, track);
    }
    prevSt = {track, playerId};
}

void ChoosingStrategy::opponentsTurn(const Track& track, int playerId) {
    updateOpponentsInfo(track, playerId);
}
