#pragma once

#include <array>
#include <vector>
#include <ostream>

#include "Point.hpp"

struct OpponentData {
    int extraField = 15;
    std::vector<Point> targetMonitors;
};

using OpponentsInfo = std::array<OpponentData, 4>;

inline std::ostream& operator<<(std::ostream& os, const OpponentData& od) {
    os << "extraField: " << od.extraField;
    os << " targets: {";
    for (const auto& x : od.targetMonitors) {
        os << x << ",";
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
