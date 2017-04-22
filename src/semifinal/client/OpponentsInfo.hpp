#pragma once

#include <array>
#include <ostream>

struct OpponentData {
    int extraField = -1;
};

using OpponentsInfo = std::array<OpponentData, 4>;

inline std::ostream& operator<<(std::ostream& os, const OpponentData& od) {
    os << "extraField: " << od.extraField;
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
