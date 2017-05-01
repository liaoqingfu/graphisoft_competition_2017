#pragma once

#include <array>
#include <ostream>
#include <set>

#include "Point.hpp"

struct Push {
    std::size_t direction;
    int position;
};

constexpr bool isValid(const Push& push) {
    return push.position >= 0;
}

constexpr Push invalidPush{0, -1};

struct OpponentData {
    int extraField = 15;
    std::set<int> targetMonitors;
    Push lastPush = invalidPush;
};

inline
bool operator==(const Push& lhs, const Push& rhs) {
    return lhs.direction == rhs.direction && lhs.position == rhs.position;
}

inline std::ostream& operator<<(std::ostream& os, const Push& push) {
    return os << push.direction << " " << push.position;
}

namespace std {

template<>
struct hash<Push> {
    std::size_t operator()(const Push& push) const {
        std::size_t seed = 0;
        hash_combine(seed, push.direction);
        hash_combine(seed, push.position);
        return seed;
    }
};

} // namespace std

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
