#ifndef SEMIFINAL_CLIENT_POTENTIALSTEP_HPP
#define SEMIFINAL_CLIENT_POTENTIALSTEP_HPP

#include "GameState.hpp"

#include <vector>

struct PotentialStep {
    const GameState* sourceState;
    Track targetTrack;
    int targetExtraField;
    Step step;
};

std::vector<PotentialStep> calculatePotentialSteps(const GameState& gameState);

#endif // SEMIFINAL_CLIENT_POTENTIALSTEP_HPP
