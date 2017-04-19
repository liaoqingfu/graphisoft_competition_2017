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

    std::sort(recordedSteps.begin(), recordedSteps.end(),
            [](const PotentialStep& lhs, const PotentialStep& rhs) {
                return lhs.weight > rhs.weight;
            });

    std::vector<PotentialStep> goodSteps;
    for (const PotentialStep& record : recordedSteps) {
        if (record.weight < recordedSteps.front().weight - 0.1) {
            break;
        }
        goodSteps.push_back(record);
    }
    std::cerr << name << ": Found " << goodSteps.size()
            << " good steps with value " << recordedSteps.front().weight
            << "\n";
    return delegatedChooser.chooseGoodStep(goodSteps);
}

#endif // SEMIFINAL_CLIENT_CHOOSERHELPER_HPP
