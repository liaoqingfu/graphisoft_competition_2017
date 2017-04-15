#ifndef SEMIFINAL_TRACK_TRACK_HPP
#define SEMIFINAL_TRACK_TRACK_HPP

#include "Matrix.hpp"

#include <iostream>

constexpr std::size_t numNeighbors = 4;
using Neighbors = std::array<Point, numNeighbors>;

constexpr Neighbors neighbors{
        {-p10, -p01, p10, p01}};

constexpr const Neighbors& getNeighbors(Point) {
    return neighbors;
}

constexpr std::size_t numFieldTypes = 16;
extern const std::array<std::string, numFieldTypes> fieldTypes;

constexpr std::size_t numPlayers = 4;
extern const std::array<std::string, numPlayers> playerColors;

extern const std::string monitorColor;
extern const std::string noColor;

struct Field {
    int type;
    bool monitor;
    int princess; // player ID, -1 = none
};

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

class Track {
public:
    using Monitors = std::vector<Point>;
    using Princesses = std::vector<Point>;

    Track(std::size_t width, std::size_t height,
            const std::vector<int>& fieldTypes,
            Monitors monitors, Princesses princesses); // TODO

    Track(const Track&) = default;
    Track(Track&&) = default;
    Track& operator=(const Track&) = default;
    Track& operator=(Track&&) = default;

    const Field& getField(Point p) const { return fields[p]; }
    Point getMonitor(int id) const { return monitors[id]; }
    Point getPrincess(int player) const { return princesses[player]; }

    void moveFields(int direction, int position); // TODO
    bool canMovePrincess(int player, Point target) const; // TODO
    void movePrincess(int player, Point target);
    void removeMonitor(int id);

private:
    Matrix<Field> fields;
    Monitors monitors;
    Princesses princesses;
};

#endif // SEMIFINAL_TRACK_TRACK_HPP
