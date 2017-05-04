#include "MMMChooser.hpp"

Step MMMChooser::chooseBadStep(
    const std::vector<PotentialStep>& potentialSteps) {

    auto changedPotentialSteps = potentialSteps;

    for (PotentialStep& step : changedPotentialSteps) {
        processStep(step);
    }
    return getDelegatedChooser().chooseBadStep(changedPotentialSteps);
}

Step MMMChooser::chooseGoodStep(
    const std::vector<PotentialStep>& potentialSteps) {

    auto changedPotentialSteps = potentialSteps;

    for (PotentialStep& step : changedPotentialSteps) {
        processStep(step);
    }

    return getDelegatedChooser().chooseGoodStep(changedPotentialSteps);
}

void MMMChooser::processStep(PotentialStep& step) {
    TemporaryStep temporaryStep{step.getGameState(), step.getStep()};
    const auto& gi = step.getGameState().gameInfo;
    const Track& track = step.getGameState().track;

    auto aliveTargets = [&]() {
        const auto& targets = gi.targets;
        const auto& alives = track.getAliveMonitors();

        // TODO unordered
        std::vector<int> result;
        for (const int t : targets) {
            if (alives.count(t)) {
                result.push_back(t);
            }
        }
        return result;
    };

    double w = 0;
    int size = track.width() + track.height();
    auto aliveTargets_ = aliveTargets();
    constexpr int N = 3;
    int i = N;
    if (aliveTargets_.size() > 1) {
        for (auto it = aliveTargets_.begin();
             it != aliveTargets_.end() && i > 0; --i) {
            Point target = track.getMonitor(*it);
            double d = std::abs(step.getStep().princessTarget.x - target.x) +
                       std::abs(step.getStep().princessTarget.y - target.y);
            d = d / size;
            w += d;
        }
    }
    w = w / aliveTargets_.size();
    w = 1.0 - w;

    double ww = w * weightMultiplier;
    step.addDebugInfo({"MMMChooser", w, ww});
    step.addWeight(ww);
}
