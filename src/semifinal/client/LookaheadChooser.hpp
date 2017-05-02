#ifndef SEMIFINAL_CLIENT_LOOKAHEADCHOOSER_HPP
#define SEMIFINAL_CLIENT_LOOKAHEADCHOOSER_HPP

#include "DelegatingChooser.hpp"
#include "GameState.hpp"
#include "PotentialStep.hpp"
#include "Track.hpp"

#include <algorithm>
#include <iostream>
#include <unordered_map>

enum class LookaheadType {
    normal, ahead, between
};

std::string to_string(LookaheadType type);

inline
std::ostream& operator<<(std::ostream& os, LookaheadType type) {
    return os << to_string(type);
}

class LookaheadChooser : public DelegatingChooser {
public:
    LookaheadChooser(std::shared_ptr<IChooser> chooser,
            LookaheadType lookahead, double weightMultiplier) :
            DelegatingChooser(std::move(chooser)),
            lookahead(lookahead),
            weightMultiplier(weightMultiplier),
            name("LookaheadChooser " + to_string(lookahead)) {
        if (debugEnabled) {
            std::cerr << "Construct: " << lookahead << "\n";
        }
    }

    Step chooseBadStep(
            const std::vector<PotentialStep>& potentialSteps) override;

private:
    void processStep(std::vector<PotentialStep>& stepValues,
            const PotentialStep& step);

    LookaheadType lookahead;
    double weightMultiplier;
    std::string name;
};

#endif // SEMIFINAL_CLIENT_LOOKAHEADCHOOSER_HPP
