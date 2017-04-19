#include "LookaheadChooser.hpp"
#include "ChooserHelper.hpp"

Step LookaheadChooser::chooseBadStep(
        const std::vector<PotentialStep>& potentialSteps) {
    return processPotentialSteps(potentialSteps,
            [this](auto& stepValues, const auto& step) {
                return processStep(stepValues, step);
            }, getDelegatedChooser(), "LookaheadChooser");
}

void LookaheadChooser::processStep(std::vector<PotentialStep>& stepValues,
        PotentialStep step) {
    std::unordered_map<Point, int> princessTargets;
    const auto& reachablePoints = step.targetTrack->getReachablePoints(
            step.targetTrack->getPrincess(step.sourceState->playerId));
    for (Point p : reachablePoints) {
        princessTargets.emplace(p, 0);
    }

    GameState newGameState{*step.sourceState, *step.targetTrack};
    newGameState.extraField = step.targetExtraField;
    for (const PotentialStep& nextStep :
            calculatePotentialSteps(newGameState)) {
        auto transformedPoints = transformPoints(*nextStep.targetTrack,
                reachablePoints, nextStep.step.pushDirection,
                nextStep.step.pushPosition);
        TransformedPointCompare comparator{&TransformedPoint::transformed};
        std::sort(transformedPoints.begin(), transformedPoints.end(),
                comparator);
        for (Point p : nextStep.targetTrack->getReachablePoints(
                nextStep.targetTrack->getMonitor(
                        step.sourceState->targetMonitor))) {
            auto iterators = std::equal_range(
                    transformedPoints.begin(), transformedPoints.end(), p,
                    comparator);
            if (iterators.first != iterators.second) {
                ++princessTargets.at(iterators.first->original);
            }
        }
    }

    for (const auto& element : princessTargets) {
        if (element.second == 0) {
            continue;
        }

        step.step.princessTarget = element.first;
        step.weight = element.second * weightMultiplier;
        stepValues.push_back(step);
    }
}
