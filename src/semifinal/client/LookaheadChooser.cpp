#include "LookaheadChooser.hpp"
#include "ChooserHelper.hpp"

#include <boost/range/adaptor/map.hpp>

Step LookaheadChooser::chooseBadStep(
        const std::vector<PotentialStep>& potentialSteps) {
    return processPotentialSteps(potentialSteps,
            [this](auto& stepValues, const auto& step) {
                return this->processStep(stepValues, step);
            }, getDelegatedChooser(), "LookaheadChooser");
}

void LookaheadChooser::processStep(std::vector<PotentialStep>& stepValues,
        const PotentialStep& step) {
    std::unordered_map<Point, int> reachablePointValues;
    // Collect the points reachable in the current step.
    const Track& targetTrack = step.getTargetTrack();
    const auto& reachablePoints = targetTrack.getReachablePoints(
            targetTrack.getPrincess(step.getSourceState().gameInfo.playerId));
    for (Point p : reachablePoints) {
        reachablePointValues.emplace(p, 0);
    }

    GameState newGameState = step.createNewGameState();
    // Iterate through the next steps.
    for (const PotentialStep& nextStep :
            calculatePotentialSteps(newGameState, step.getOpponentInfo())) {
        const Track& nextTargetTrack = nextStep.getTargetTrack();
        // Transform reachablePoints (save both original and transformed points)
        auto transformedPoints = transformPoints(nextTargetTrack,
                reachablePoints, nextStep.getStep().pushDirection,
                nextStep.getStep().pushPosition);
        TransformedPointCompare comparator{&TransformedPoint::transformed};
        // Sort for binary search.
        std::sort(transformedPoints.begin(), transformedPoints.end(),
                comparator);
        // Find the common subset of the points reachable from the target
        // monitor after the next step and the points reachable by the princess
        // in the current step.
        for (Point p : nextTargetTrack.getReachablePoints(
                nextTargetTrack.getMonitor(
                        step.getSourceState().targetMonitor))) {

            auto iterators = std::equal_range(
                    transformedPoints.begin(), transformedPoints.end(), p,
                    comparator);
            if (iterators.first != iterators.second) {
                ++reachablePointValues.at(iterators.first->original);
            }
        }
    }

    auto valueRange = reachablePointValues | boost::adaptors::map_values;
    double valueLimit = *std::max_element(valueRange.begin(),
            valueRange.end()) + 1.0;

    for (const auto& element : reachablePointValues) {
        if (element.second == 0) {
            continue;
        }

        PotentialStep step2 = step;
        step2.setPrincessTarget(element.first);
        // The value equals the number of possible next steps where the princess
        // can reach the target monitor.
        double w = element.second / valueLimit;
        double ww = w * weightMultiplier;
        step2.addWeight(ww);
        step2.debugInfo.push_back(PotentialStep::DebugInfo{"LookaheadChooser", w, ww});
        stepValues.push_back(std::move(step2));
    }
}
