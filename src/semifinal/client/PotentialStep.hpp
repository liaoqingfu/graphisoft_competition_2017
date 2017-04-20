#ifndef SEMIFINAL_CLIENT_POTENTIALSTEP_HPP
#define SEMIFINAL_CLIENT_POTENTIALSTEP_HPP

#include "GameState.hpp"

#include <memory>
#include <vector>

struct PotentialStep {
    const GameState* sourceState;
    // The track should be const because it may be shared between objects.
    std::shared_ptr<const Track> targetTrack;
    int targetExtraField;
    Step step;
    double weight;
};

std::vector<PotentialStep> calculatePotentialSteps(const GameState& gameState,
                                                   int playerId,
                                                   int extraField);
std::vector<PotentialStep> calculatePotentialSteps(const GameState& gameState);

#endif // SEMIFINAL_CLIENT_POTENTIALSTEP_HPP
