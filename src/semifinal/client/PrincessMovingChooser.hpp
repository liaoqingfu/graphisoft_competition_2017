#ifndef SEMIFINAL_CLIENT_PRINCESSMOVINGCHOOSER_HPP
#define SEMIFINAL_CLIENT_PRINCESSMOVINGCHOOSER_HPP

#include "DelegatingChooser.hpp"
#include "GameState.hpp"
#include "PotentialStep.hpp"
#include "Track.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <unordered_map>

class PrincessMovingChooser : public DelegatingChooser {
public:
    PrincessMovingChooser(std::unique_ptr<IChooser>&& chooser) :
            DelegatingChooser(std::move(chooser)) {}

    Step chooseBadStep(
            const std::vector<PotentialStep>& potentialSteps) override {
        std::vector<StepRecord> stepValues;
        for (const PotentialStep& step : potentialSteps) {
            processStep(stepValues, step);
        }
        if (stepValues.empty()) {
            std::cerr << "PrincessMovingChooser: No good step is found.\n";
            return DelegatingChooser::chooseBadStep(potentialSteps);
        }

        std::sort(stepValues.begin(), stepValues.end(),
                [](const StepRecord& lhs, const StepRecord& rhs) {
                    return lhs.value < rhs.value;
                });

        std::vector<PotentialStep> goodSteps;
        for (const StepRecord& record : stepValues) {
            if (record.value != stepValues.front().value) {
                break;
            }
            goodSteps.push_back(record.step);
        }
        std::cerr << "PrincessMovingChooser: Found " << goodSteps.size()
                << " good steps with value " << stepValues.front().value
                << "\n";
        return DelegatingChooser::chooseGoodStep(goodSteps);
    }

private:
    struct StepRecord {
        PotentialStep step;
        int value;
    };

    void processStep(std::vector<StepRecord>& stepValues, PotentialStep step) {
        const Track& track = step.targetTrack;
        int playerId = step.sourceState->playerId;
        Point target = track.getMonitor(playerId);

        for (Point p : track.getReachablePoints(track.getPrincess(playerId))) {
            step.step.princessTarget = p;
            stepValues.push_back(StepRecord{step,
                    std::abs(p.x - target.x) + std::abs(p.y - target.y)});
        }
    }
};

#endif // SEMIFINAL_CLIENT_PRINCESSMOVINGCHOOSER_HPP
