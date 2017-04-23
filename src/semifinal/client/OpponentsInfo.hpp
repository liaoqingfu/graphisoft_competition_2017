#pragma once

#include <array>
#include <ostream>
#include <set>

#include "Point.hpp"

struct OpponentData {
    int extraField = 15;
    std::set<int> targetMonitors;
};

using OpponentsInfo = std::array<OpponentData, 4>;

inline std::ostream& operator<<(std::ostream& os, const OpponentData& od) {
    os << "extraField: " << od.extraField;
    os << "   targets: {";
    for (const auto& x : od.targetMonitors) {
        os << "M" << x << ",";
    }
    os << "}";
    return os;
}

inline std::ostream& operator<<(std::ostream& os,
                                const OpponentsInfo& opponentsInfo) {
    int player = 0;
    for (const auto& x : opponentsInfo) {
        os << "player" << player << ": " << x << "\n";
        ++player;
    }
    return os;
}
