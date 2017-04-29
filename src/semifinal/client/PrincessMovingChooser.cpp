#include "PrincessMovingChooser.hpp"
#include "ChooserHelper.hpp"

Step PrincessMovingChooser::chooseGoodStep(
        const std::vector<PotentialStep>& potentialSteps) {
    if (overrideGoodSteps) {
        auto steps = potentialSteps;
        for (PotentialStep& step : steps) {
            step.addWeight(calculateWeight(step));
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
                return this->processStep(stepValues, step, true);
            }, getDelegatedChooser(), "PrincessMovingChooser");
}

void PrincessMovingChooser::processStep(std::vector<PotentialStep>& stepValues,
        const PotentialStep& step, bool isBadStep) {
    const Track& track = step.getTargetTrack();
    int playerId = step.getSourceState().gameInfo.playerId;

    for (Point p : track.getReachablePoints(track.getPrincess(playerId))) {
        PotentialStep newStep = step;
        newStep.setPrincessTarget(p);
        double weight = calculateWeight(newStep);
        if (isBadStep) {
            weight *= 5;
        }
        newStep.addWeight(weight);
        stepValues.push_back(std::move(newStep));
    }
}

double PrincessMovingChooser::calculateWeight(PotentialStep& step) {
    const Track& track = step.getTargetTrack();
    Point target = track.getMonitor(step.getSourceState().targetMonitor);

    int size = track.width() + track.height();
    double d = std::abs(step.getStep().princessTarget.x - target.x)
                + std::abs(step.getStep().princessTarget.y - target.y);
    // std::cerr << "|" << step.step.princessTarget << " - " << target << "| = "
    //         << d << "\n";
    double w = (1.0 - d / static_cast<double>(size));
    double ww = w * weightMultiplier;
    step.addDebugInfo({"PrincessMovingChooser", w, ww});
    double result = ww;
    // std::cerr << step.step << ": (1 - " << d << " / " << size << ") * "
    //         << weightMultiplier << " = " << result << "\n";
    return result;

}
