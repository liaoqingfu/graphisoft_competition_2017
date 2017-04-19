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
    LookaheadChooser(std::unique_ptr<IChooser>&& chooser) :
            DelegatingChooser(std::move(chooser)) {}

    Step chooseBadStep(
            const std::vector<PotentialStep>& potentialSteps) override;

private:
    void processStep(std::vector<PotentialStep>& stepValues,
            PotentialStep step);
};

#endif // SEMIFINAL_CLIENT_LOOKAHEADCHOOSER_HPP
