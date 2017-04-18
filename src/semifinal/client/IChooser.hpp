#ifndef SEMIFINAL_CLIENT_ICHOOSER_HPP
#define SEMIFINAL_CLIENT_ICHOOSER_HPP

#include "PotentialStep.hpp"

class IChooser {
public:
    virtual Step chooseGoodStep(
            const std::vector<PotentialStep>& potentialSteps) = 0;
    virtual Step chooseBadStep(
            const std::vector<PotentialStep>& potentialSteps) = 0;
    virtual ~IChooser() {}
};

#endif // SEMIFINAL_CLIENT_ICHOOSER_HPP
