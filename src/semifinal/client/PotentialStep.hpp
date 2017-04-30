#ifndef SEMIFINAL_CLIENT_POTENTIALSTEP_HPP
#define SEMIFINAL_CLIENT_POTENTIALSTEP_HPP

#include "GameState.hpp"
#include "OpponentsInfo.hpp"

#include <memory>
#include <vector>

class PotentialStep {
public:
    struct DebugInfo {
        std::string name;
        double baseValue;
        double finalValue;
    };

    PotentialStep() : gameState(nullptr), opponentsInfo(nullptr) {
        assert(false);
    }

    PotentialStep(const GameState& gameState,
            const OpponentsInfo& opponentsInfo,
            Step step, int /*playerId*/) :
            gameState(&gameState), opponentsInfo(&opponentsInfo),
            step(std::move(step)) {}

    PotentialStep(const PotentialStep&) = default;
    PotentialStep& operator=(const PotentialStep&) = default;
    PotentialStep(PotentialStep&&) = default;
    PotentialStep& operator=(PotentialStep&&) = default;

    const GameState& getGameState() const { return *gameState; }
    const OpponentsInfo& getOpponentInfo() const { return *opponentsInfo; }
    const Step& getStep() const { return step; }
    void setPrincessTarget(Point target) {
        step.princessTarget = target;
    }

    double getWeight() const { return weight; }
    void addWeight(double value) { weight += value; }

    void addDebugInfo(DebugInfo info) {
        if (debugEnabled) {
            debugInfo.push_back(std::move(info));
        }
    }

    void printDebugInfo(std::ostream& stream, const char* prefix) const {
        for (const auto& info : debugInfo) {
            stream << prefix << info.name << ": base " << info.baseValue
                    << " weighted " << info.finalValue << "\n";
        }
    }

    static bool debugEnabled;

private:
    const GameState* gameState;
    const OpponentsInfo* opponentsInfo;
    Step step;

    double weight = 1.0;

    std::vector<DebugInfo> debugInfo;
};

std::vector<PotentialStep> calculatePotentialSteps(
    const GameState& gameState, const OpponentsInfo& opponentsInfo,
    int playerId, int extraField);
std::vector<PotentialStep> calculatePotentialSteps(
    const GameState& gameState, const OpponentsInfo& opponentsInfo);

#endif // SEMIFINAL_CLIENT_POTENTIALSTEP_HPP
