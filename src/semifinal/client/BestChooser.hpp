#ifndef SEMIFINAL_CLIENT_BESTCHOOSER_HPP
#define SEMIFINAL_CLIENT_BESTCHOOSER_HPP

#include "DelegatingChooser.hpp"

#include <iostream>

class BestChooser : public DelegatingChooser {
public:
    BestChooser(std::shared_ptr<IChooser> chooser, double tolerance = 0.01) :
            DelegatingChooser(std::move(chooser)),
            tolerance(tolerance) {}

    Step chooseGoodStep(
            const std::vector<PotentialStep>& potentialSteps) override {
        return DelegatingChooser::chooseGoodStep(
                findBestPosition(potentialSteps));
    }

    Step chooseBadStep(
            const std::vector<PotentialStep>& potentialSteps) override {
        return DelegatingChooser::chooseBadStep(
                findBestPosition(potentialSteps));
    }

private:
    std::vector<PotentialStep> findBestPosition(
            std::vector<PotentialStep> potentialSteps) {
        std::sort(potentialSteps.begin(), potentialSteps.end(),
                [](const PotentialStep& lhs, const PotentialStep& rhs) {
                    return lhs.getWeight() > rhs.getWeight();
                });
        std::size_t i = 0;
        bool over = false;
        for (const PotentialStep& step : potentialSteps) {
            if (!over && step.getWeight() <
                    potentialSteps[0].getWeight() - tolerance) {
                over = true;
                std::cerr << "-----------------------------\n";
            }
            std::cerr << "BestChooser " << step.getStep()
                    << " " << step.getWeight() << "\n";
            for (const auto& info : step.debugInfo) {
                std::cerr << "  " << info.name << " " << info.baseValue
                        << " " << info.finalValue << "\n";
            }
        }
        for (; i < potentialSteps.size() &&
                potentialSteps[i].getWeight() >=
                        potentialSteps[0].getWeight() - tolerance;
                ++i) {
            // std::cerr << "-> " << potentialSteps[i].step.pushDirection << " "
            //         << potentialSteps[i].step.pushPosition << " "
            //         << potentialSteps[i].step.princessTarget << ": "
            //         << potentialSteps[i].weight << "\n";
        }
        potentialSteps.resize(i);
        std::cerr << "Filtered out the " << i << " best steps\n";
        return potentialSteps;
    }

    double tolerance;
};

#endif // SEMIFINAL_CLIENT_BESTCHOOSER_HPP
