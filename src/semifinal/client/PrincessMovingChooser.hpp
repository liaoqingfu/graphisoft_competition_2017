#ifndef SEMIFINAL_CLIENT_PRINCESSMOVINGCHOOSER_HPP
#define SEMIFINAL_CLIENT_PRINCESSMOVINGCHOOSER_HPP

#include "DelegatingChooser.hpp"
#include "GameState.hpp"
#include "PotentialStep.hpp"
#include "Track.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <unordered_map>

class PrincessMovingChooser : public DelegatingChooser {
public:
    PrincessMovingChooser(std::unique_ptr<IChooser>&& chooser) :
            DelegatingChooser(std::move(chooser)) {}

    Step chooseBadStep(
            const std::vector<PotentialStep>& potentialSteps) override;

private:
    void processStep(std::vector<PotentialStep>& stepValues, PotentialStep step);
};

#endif // SEMIFINAL_CLIENT_PRINCESSMOVINGCHOOSER_HPP
