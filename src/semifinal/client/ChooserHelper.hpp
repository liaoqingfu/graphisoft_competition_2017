#ifndef SEMIFINAL_CLIENT_CHOOSERHELPER_HPP
#define SEMIFINAL_CLIENT_CHOOSERHELPER_HPP

#include "IChooser.hpp"
#include "PotentialStep.hpp"

#include <vector>

template<typename ProcessStep>
Step processPotentialSteps(const std::vector<PotentialStep>& potentialSteps,
        const ProcessStep& processStep, IChooser& delegatedChooser,
        const char* name) {
    std::vector<PotentialStep> recordedSteps;
    for (const PotentialStep& step : potentialSteps) {
        processStep(recordedSteps, step);
    }
    if (recordedSteps.empty()) {
        std::cerr << name << ": No good step is found.\n";
        return delegatedChooser.chooseBadStep(potentialSteps);
    }

    std::cerr << name << ": Found " << recordedSteps.size() << " good steps\n";
    return delegatedChooser.chooseGoodStep(recordedSteps);
}

#endif // SEMIFINAL_CLIENT_CHOOSERHELPER_HPP
