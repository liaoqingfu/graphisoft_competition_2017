#include "LookaheadChooser.hpp"
#include "ChooserHelper.hpp"

#include <boost/range/adaptor/map.hpp>

Step LookaheadChooser::chooseBadStep(
        const std::vector<PotentialStep>& potentialSteps) {
    return processPotentialSteps(potentialSteps,
            [this](auto& stepValues, const auto& step) {
                return processStep(stepValues, step);
            }, getDelegatedChooser(), "LookaheadChooser");
}

namespace {

template<typename ReachablePoints>
void calculateTargetValues(const GameState& gameState,
        const PotentialStep& step,
        const ReachablePoints& reachablePoints,
        std::unordered_map<Point, int>& reachablePointValues) {
    // Iterate through the next steps.
    for (const PotentialStep& nextStep :
            calculatePotentialSteps(gameState, *step.opponentsInfo)) {
        // Transform reachablePoints (save both original and transformed points)
        auto transformedPoints = transformPoints(*nextStep.targetTrack,
                reachablePoints, nextStep.step.pushDirection,
                nextStep.step.pushPosition);
        TransformedPointCompare comparator{&TransformedPoint::transformed};
        // Sort for binary search.
        std::sort(transformedPoints.begin(), transformedPoints.end(),
                comparator);
        // Find the common subset of the points reachable from the target
        // monitor after the next step and the points reachable by the princess
        // in the current step.
        for (Point p : nextStep.targetTrack->getReachablePoints(
                nextStep.targetTrack->getMonitor(
                        step.sourceState->targetMonitor))) {

            auto iterators = std::equal_range(
                    transformedPoints.begin(), transformedPoints.end(), p,
                    comparator);
            if (iterators.first != iterators.second) {
                ++reachablePointValues.at(iterators.first->original);
            }
        }
    }
}

} // unnamed namespace

void LookaheadChooser::processStep(std::vector<PotentialStep>& stepValues,
        PotentialStep step) {
    std::unordered_map<Point, int> reachablePointValues;
    // Collect the points reachable in the current step.
    const auto& reachablePoints = step.targetTrack->getReachablePoints(
            step.targetTrack->getPrincess(step.sourceState->gameInfo.playerId));
    for (Point p : reachablePoints) {
        reachablePointValues.emplace(p, 0);
    }

    GameState newGameState{*step.sourceState, *step.targetTrack};
    newGameState.extraField = step.targetExtraField;

    if (lookahead == 1) {
        calculateTargetValues(newGameState, step, reachablePoints,
                reachablePointValues);
    } else {
        for (const PotentialStep& nextStep :
                calculatePotentialSteps(newGameState, *step.opponentsInfo)) {
            auto transformedPoints = transformPoints(*nextStep.targetTrack,
                    reachablePoints, nextStep.step.pushDirection,
                    nextStep.step.pushPosition);
            std::size_t max = transformedPoints.size();
            for (std::size_t i = 0; i < max; ++i) {
                for (Point target : nextStep.targetTrack->getReachablePoints(
                        transformedPoints[i].transformed)) {
                    transformedPoints.push_back(TransformedPoint{
                            transformedPoints[i].original, target});
                }
            }

            GameState newGameState{*nextStep.sourceState,
                    *nextStep.targetTrack};
            newGameState.extraField = step.targetExtraField;
            calculateTargetValues(newGameState, nextStep,
                    transformedPoints, reachablePointValues);
        }
    }

    auto valueRange = reachablePointValues | boost::adaptors::map_values;
    double valueLimit = *std::max_element(valueRange.begin(),
            valueRange.end()) + 1.0;

    double baseWeight = step.weight;
    for (const auto& element : reachablePointValues) {
        if (element.second == 0) {
            continue;
        }

        step.step.princessTarget = element.first;
        // The value equals the number of possible next steps where the princess
        // can reach the target monitor.
        step.weight = baseWeight
                + element.second / valueLimit * weightMultiplier;
        stepValues.push_back(step);
    }
}
