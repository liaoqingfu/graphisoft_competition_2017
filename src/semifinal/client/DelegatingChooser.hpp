#ifndef SEMIFINAL_CLIENT_DELEGATINGCHOOSER_HPP
#define SEMIFINAL_CLIENT_DELEGATINGCHOOSER_HPP

#include "IChooser.hpp"

#include <memory>

class DelegatingChooser : public IChooser {
public:
    DelegatingChooser(std::unique_ptr<IChooser>&& chooser) :
            chooser(std::move(chooser)) {}

    Step chooseGoodStep(
            const std::vector<PotentialStep>& potentialSteps) override {
        return chooser->chooseGoodStep(potentialSteps);
    }

    Step chooseBadStep(
            const std::vector<PotentialStep>& potentialSteps) override {
        return chooser->chooseBadStep(potentialSteps);
    }

protected:
    IChooser& getDelegatedChooser() { return *chooser; }

private:
    std::unique_ptr<IChooser> chooser;
};

#endif // SEMIFINAL_CLIENT_DELEGATINGCHOOSER_HPP
