#ifndef SEMIFINAL_CLIENT_POTENTIALSTEP_HPP
#define SEMIFINAL_CLIENT_POTENTIALSTEP_HPP

#include "GameState.hpp"
#include "OpponentsInfo.hpp"

#include <memory>
#include <vector>

struct PotentialStep {
    const GameState* sourceState;
    const OpponentsInfo* opponentsInfo;
    // The track should be const because it may be shared between objects.
    std::shared_ptr<const Track> targetTrack;
    int targetExtraField;
    Step step;
    double weight;

    struct DebugInfo {
        std::string name;
        double baseValue;
        double finalValue;
    };
    std::vector<DebugInfo> debugInfo;
};

std::vector<PotentialStep> calculatePotentialSteps(
    const GameState& gameState, const OpponentsInfo& opponentsInfo,
    int playerId, int extraField);
std::vector<PotentialStep> calculatePotentialSteps(
    const GameState& gameState, const OpponentsInfo& opponentsInfo);

#endif // SEMIFINAL_CLIENT_POTENTIALSTEP_HPP
