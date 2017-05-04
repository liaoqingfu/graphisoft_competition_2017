#include "LookaheadChooser.hpp"
#include "ChooserHelper.hpp"
#include "Transformations.hpp"

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/range/adaptor/map.hpp>

Step LookaheadChooser::chooseBadStep(
        const std::vector<PotentialStep>& potentialSteps) {
    boost::posix_time::ptime limit =
            boost::posix_time::microsec_clock::universal_time()
                    + boost::posix_time::millisec(1000);
    return processPotentialSteps(potentialSteps,
            [this, limit](auto& stepValues, const auto& step) {
                if (boost::posix_time::microsec_clock::universal_time()
                        <= limit) {
                    this->processStep(stepValues, step);
                }
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

void processAhehadStrategy(const PotentialStep& step,
        std::vector<Point>& reachablePoints,
        std::vector<int>& reachablePointValues) {
    const GameState& gameState = step.getGameState();
    const Track& track = gameState.track;
    for (const PotentialStep& nextStep : calculatePotentialSteps(
                gameState, step.getOpponentInfo())) {
        TemporaryStep temporaryStep2{gameState, nextStep.getStep(),
                reachablePoints};
        std::unordered_map<Point, std::vector<std::size_t>>
                transformations;
        std::vector<Point> transformedPoints;
        for (std::size_t i = 0; i < reachablePoints.size(); ++i) {
            for (Point target : track.getReachablePoints(
                    reachablePoints[i])) {
                auto iterator = transformations.find(target);
                if (iterator == transformations.end()) {
                    iterator = transformations.emplace(target,
                            std::vector<std::size_t>{}).first;
                    transformedPoints.push_back(target);
                }
                iterator->second.push_back(i);
            }
        }

        std::vector<int> collectedValues(transformedPoints.size(), 0);
        calculateTargetValues(nextStep, transformedPoints,
                [&collectedValues](std::size_t i) {
                    ++collectedValues[i];
                });
        for (std::size_t i = 0; i < transformedPoints.size(); ++i) {
            for (std::size_t j : transformations.at(
                    transformedPoints[i])) {
                reachablePointValues[j] += collectedValues[i];
            }
        }
    }
}

int calculateBetweenWeight(const PotentialStep& step,
        Point princessTarget) {
    const GameState& gameState = step.getGameState();
    const Track& track = gameState.track;
    int result = 1;
    std::vector<Point> target{princessTarget};
    for (const PotentialStep& nextStep : calculatePotentialSteps(
                gameState, step.getOpponentInfo())) {
        TemporaryStep temporaryStep2{gameState, nextStep.getStep(), target};
        if (track.isReachableFrom(target[0],
                track.getMonitor(gameState.targetMonitor))) {
            result += 1;
        }
    }
    return result;
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

        if (lookahead == LookaheadType::ahead) {
            processAhehadStrategy(step, reachablePoints, reachablePointValues);
        } else {
            calculateTargetValues(step, reachablePoints,
                    [&reachablePointValues](std::size_t i) {
                        ++reachablePointValues[i];
                    });
            if (lookahead == LookaheadType::between) {
                for (std::size_t i = 0; i < reachablePoints.size(); ++i) {
                    if (reachablePointValues[i] != 0) {
                        reachablePointValues[i] = calculateBetweenWeight(
                                step, reachablePoints[i]);
                    }
                }
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

std::string to_string(LookaheadType type) {
    switch (type) {
    case LookaheadType::normal: return "normal";
    case LookaheadType::ahead: return "ahead";
    case LookaheadType::between: return "between";
    default: return "none";
    }
}

