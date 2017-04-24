#include "PrincessMovingChooser.hpp"
#include "ChooserHelper.hpp"

Step PrincessMovingChooser::chooseGoodStep(
        const std::vector<PotentialStep>& potentialSteps) {
    if (overrideGoodSteps) {
        auto steps = potentialSteps;
        for (PotentialStep& step : steps) {
            step.weight += calculateWeight(step);
        }
        return DelegatingChooser::chooseGoodStep(steps);
    } else {
        return DelegatingChooser::chooseGoodStep(potentialSteps);
    }
}

Step PrincessMovingChooser::chooseBadStep(
        const std::vector<PotentialStep>& potentialSteps) {
    return processPotentialSteps(potentialSteps,
            [this](auto& stepValues, const auto& step) {
                return processStep(stepValues, step);
            }, getDelegatedChooser(), "PrincessMovingChooser");
}

void PrincessMovingChooser::processStep(std::vector<PotentialStep>& stepValues,
        const PotentialStep& step) {
    const Track& track = *step.targetTrack;
    int playerId = step.sourceState->gameInfo.playerId;

    double baseWeight = step.weight;
    for (Point p : track.getReachablePoints(track.getPrincess(playerId))) {
        auto step2 = step;
        step2.step.princessTarget = p;
        step2.weight = baseWeight + calculateWeight(step2);
        stepValues.push_back(step2);
    }
}

double PrincessMovingChooser::calculateWeight(PotentialStep& step) {
    const Track& track = *step.targetTrack;
    Point target = track.getMonitor(step.sourceState->targetMonitor);

    int size = track.width() + track.height();
    double d = std::abs(step.step.princessTarget.x - target.x)
                + std::abs(step.step.princessTarget.y - target.y);
    // std::cerr << "|" << step.step.princessTarget << " - " << target << "| = "
    //         << d << "\n";
    double w = (1.0 - d / static_cast<double>(size));
    double ww = w * weightMultiplier;
    step.debugInfo.push_back({"PrincessMovingChooser", w, ww});
    double result = ww;
    // std::cerr << step.step << ": (1 - " << d << " / " << size << ") * "
    //         << weightMultiplier << " = " << result << "\n";
    return result;

}
