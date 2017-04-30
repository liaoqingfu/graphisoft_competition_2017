#ifndef SEMIFINAL_CLIENT_LOOKAHEADCHOOSER_HPP
#define SEMIFINAL_CLIENT_LOOKAHEADCHOOSER_HPP

#include "DelegatingChooser.hpp"
#include "GameState.hpp"
#include "PotentialStep.hpp"
#include "Track.hpp"

#include <algorithm>
#include <iostream>
#include <unordered_map>

class LookaheadChooser : public DelegatingChooser {
public:
    LookaheadChooser(std::shared_ptr<IChooser> chooser,
            int lookahead, double weightMultiplier) :
            DelegatingChooser(std::move(chooser)),
            lookahead(lookahead),
            weightMultiplier(weightMultiplier) {
        std::cerr << "Construct: " << lookahead << "\n";
    }

    Step chooseBadStep(
            const std::vector<PotentialStep>& potentialSteps) override;

private:
    void processStep(std::vector<PotentialStep>& stepValues,
            PotentialStep step);

    int lookahead;
    double weightMultiplier;
};

#endif // SEMIFINAL_CLIENT_LOOKAHEADCHOOSER_HPP
