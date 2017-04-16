#ifndef SEMIFINAL_TRACK_FIELD_HPP
#define SEMIFINAL_TRACK_FIELD_HPP

#include "Point.hpp"

#include <array>
#include <assert.h>
#include <vector>

constexpr std::size_t numNeighbors = 4;
using Neighbors = std::array<Point, numNeighbors>;

inline constexpr
std::size_t oppositeDirection(std::size_t direction) {
    return (direction + numNeighbors / 2) % numNeighbors;
}

enum Directions : std::size_t {
    left, up, right, down
};

constexpr Neighbors neighbors{
        {-p10, -p01, p10, p01}};

constexpr const Neighbors& getNeighbors(Point) {
    return neighbors;
}

struct Field {
    int type;
    bool monitor;
    int princess; // player ID, -1 = none
};

inline
bool operator==(const Field& lhs, const Field& rhs) {
    return lhs.type == rhs.type && lhs.monitor == rhs.monitor
            && lhs.princess == rhs.princess;
}

std::string to_string(const Field& field);

inline
std::ostream& operator<<(std::ostream& os, const Field& field) {
    return os << to_string(field);
}

inline
bool canMove(int fieldType, std::size_t direction) {
    constexpr std::array<int, numNeighbors> directionBits{{
            2, 1, 8, 4}};
    assert(direction <  numNeighbors);
    return fieldType & directionBits[direction];
}

std::vector<int> getIsomorphs(int fieldType); // TODO

#endif // SEMIFINAL_TRACK_FIELD_HPP
