#include "LookaheadChooser.hpp"
#include "ChooserHelper.hpp"
#include "Transformations.hpp"

#include <boost/range/adaptor/map.hpp>

Step LookaheadChooser::chooseBadStep(
        const std::vector<PotentialStep>& potentialSteps) {
    return processPotentialSteps(potentialSteps,
            [this](auto& stepValues, const auto& step) {
                return this->processStep(stepValues, step);
            }, getDelegatedChooser(), name.c_str());
}

namespace {

template<typename AddValue>
void calculateTargetValues(
        const PotentialStep& step,
        std::vector<Point>& reachablePoints,
        const AddValue& addValue) {
    const GameState& gameState = step.getGameState();
    // Iterate through the next steps.
    for (const PotentialStep& nextStep :
            calculatePotentialSteps(gameState, step.getOpponentInfo())) {
        TemporaryStep temporaryStep{gameState, nextStep.getStep(),
                reachablePoints};
        const Track& track = gameState.track;
        auto reachableFromTarget = track.getReachablePoints(
                track.getMonitor(gameState.targetMonitor));
        std::sort(reachableFromTarget.begin(), reachableFromTarget.end());
        // Find the common subset of the points reachable from the target
        // monitor after the next step and the points reachable by the princess
        // in the current step.
        for (std::size_t i = 0; i < reachablePoints.size(); ++i) {
            auto iterators = std::equal_range(
                    reachableFromTarget.begin(), reachableFromTarget.end(),
                    reachablePoints[i]);
            if (iterators.first != iterators.second) {
                addValue(i);
            }
        }
    }
}

} // unnamed namespace

void LookaheadChooser::processStep(std::vector<PotentialStep>& stepValues,
        const PotentialStep& step) {
    const GameState& gameState = step.getGameState();
    std::vector<Point> reachablePoints;
    std::vector<int> reachablePointValues;
    {
        TemporaryStep temporaryStep1{gameState, step.getStep()};
        const Track& track = gameState.track;
        // Collect the points reachable in the current step.
        reachablePoints = track.getReachablePoints(
                track.getPrincess(step.getGameState().gameInfo.playerId));
        reachablePointValues.resize(reachablePoints.size(), 0);

        if (lookahead == 1) {
            calculateTargetValues(step, reachablePoints,
                    [&reachablePointValues](std::size_t i) {
                        ++reachablePointValues[i];
                    });
        } else {
            for (const PotentialStep& nextStep : calculatePotentialSteps(
                        gameState, step.getOpponentInfo())) {
                TemporaryStep temporaryStep2{gameState, nextStep.getStep(),
                        reachablePoints};
                std::vector<std::size_t> transformations;
                std::vector<Point> transformedPoints;
                for (std::size_t i = 0; i < reachablePoints.size(); ++i) {
                    for (Point target : track.getReachablePoints(
                            reachablePoints[i])) {
                        transformations.push_back(i);
                        transformedPoints.push_back(target);
                    }
                }

                calculateTargetValues(nextStep, transformedPoints,
                        [&reachablePointValues, &transformations](
                                std::size_t i) {
                            ++reachablePointValues[transformations[i]];
                        });
            }
        }
    }

    double valueLimit = *std::max_element(reachablePointValues.begin(),
            reachablePointValues.end()) + 1.0;

    for (std::size_t i = 0; i < reachablePointValues.size(); ++i) {
        if (reachablePointValues[i] == 0) {
            continue;
        }

        PotentialStep step2 = step;
        step2.setPrincessTarget(reachablePoints[i]);
        // The value equals the number of possible next steps where the princess
        // can reach the target monitor.
        double w = reachablePointValues[i] / valueLimit;
        double ww = w * weightMultiplier;
        step2.addWeight(ww);
        step2.addDebugInfo(PotentialStep::DebugInfo{"LookaheadChooser", w, ww});
        stepValues.push_back(std::move(step2));
    }
}
