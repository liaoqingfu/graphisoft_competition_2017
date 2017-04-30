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
    {
        TemporaryStep temporaryStep1{step.getGameState(), step.getStep()};
        const Track& track = step.getGameState().track;
        const auto& reachablePoints = track.getReachablePoints(
                track.getPrincess(step.getGameState().gameInfo.playerId));
        for (Point p : reachablePoints) {
            reachablePointValues.emplace(p, 0);
        }

        // Iterate through the next steps.
        for (const PotentialStep& nextStep :
                calculatePotentialSteps(step.getGameState(),
                        step.getOpponentInfo())) {
            TemporaryStep temporaryStep2{step.getGameState(),
                    nextStep.getStep()};
            // Transform reachablePoints (save both original and transformed
            // points)
            auto transformedPoints = transformPoints(track,
                    reachablePoints, nextStep.getStep().pushDirection,
                    nextStep.getStep().pushPosition);
            TransformedPointCompare comparator{&TransformedPoint::transformed};
            // Sort for binary search.
            std::sort(transformedPoints.begin(), transformedPoints.end(),
                    comparator);
            // Find the common subset of the points reachable from the target
            // monitor after the next step and the points reachable by the
            // princess in the current step.
            for (Point p : track.getReachablePoints(
                    track.getMonitor(
                            step.getGameState().targetMonitor))) {

                auto iterators = std::equal_range(
                        transformedPoints.begin(), transformedPoints.end(), p,
                        comparator);
                if (iterators.first != iterators.second) {
                    ++reachablePointValues.at(iterators.first->original);
                }
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
        step2.addDebugInfo(PotentialStep::DebugInfo{"LookaheadChooser", w, ww});
        stepValues.push_back(std::move(step2));
    }
}
