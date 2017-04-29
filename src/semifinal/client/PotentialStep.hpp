#ifndef SEMIFINAL_CLIENT_POTENTIALSTEP_HPP
#define SEMIFINAL_CLIENT_POTENTIALSTEP_HPP

#include "GameState.hpp"
#include "OpponentsInfo.hpp"

#include <memory>
#include <vector>

class PotentialStep {
public:
    PotentialStep() : sourceState(nullptr), opponentsInfo(nullptr) {
        assert(false);
    }

    PotentialStep(const GameState& sourceState,
            const OpponentsInfo& opponentsInfo,
            Step step, int playerId) :
            sourceState(&sourceState), opponentsInfo(&opponentsInfo),
            step(std::move(step)),
            playerId(playerId),
            targetTrack(std::make_shared<Track>(sourceState.track)),
            targetExtraField(executeStep(*targetTrack, playerId, step)) {
    }

    PotentialStep(const PotentialStep&) = default;
    PotentialStep& operator=(const PotentialStep&) = default;
    PotentialStep(PotentialStep&&) = default;
    PotentialStep& operator=(PotentialStep&&) = default;

    const GameState& getSourceState() const { return *sourceState; }
    const OpponentsInfo& getOpponentInfo() const { return *opponentsInfo; }
    const Step& getStep() const { return step; }
    void setPrincessTarget(Point target) {
        step.princessTarget = target;
        if (targetTrack) {
            targetTrack->movePrincess(playerId, target);
        }
    }

    const Track& getTargetTrack() const {
        return *targetTrack;
    }

    int getTargetExtraField() const {
        return targetExtraField;
    }

    double getWeight() const { return weight; }
    void addWeight(double value) { weight += value; }

    GameState createNewGameState() const {
        GameState newGameState{*sourceState, *targetTrack};
        newGameState.extraField = targetExtraField;
        return newGameState;
    }

    struct DebugInfo {
        std::string name;
        double baseValue;
        double finalValue;
    };
    std::vector<DebugInfo> debugInfo;
private:
    const GameState* sourceState;
    const OpponentsInfo* opponentsInfo;
    Step step;
    int playerId;

    double weight = 1.0;

    std::shared_ptr<Track> targetTrack;
    int targetExtraField;
};

std::vector<PotentialStep> calculatePotentialSteps(
    const GameState& gameState, const OpponentsInfo& opponentsInfo,
    int playerId, int extraField);
std::vector<PotentialStep> calculatePotentialSteps(
    const GameState& gameState, const OpponentsInfo& opponentsInfo);

#endif // SEMIFINAL_CLIENT_POTENTIALSTEP_HPP
