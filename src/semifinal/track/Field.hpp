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
    int type = 0;
    int monitor = -1; // monitor ID, -1 = none

    std::vector<int> princess;
    void removePrincess(int p) {
        princess.erase(std::remove(princess.begin(), princess.end(), p),
                       princess.end());
    }
    void addPrincess(int p) { princess.push_back(p); }
    bool hasPrincess(int p) const {
        return std::find(princess.begin(), princess.end(), p) != princess.end();
    }
    bool noPrincess() const {
        return princess.empty();
    }
    const std::vector<int>& getPrincess() const { return princess; }
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

constexpr std::size_t numFieldTypes = 16;
extern const std::array<std::string, numFieldTypes> fieldTypes;

constexpr std::size_t numPlayers = 4;
constexpr std::array<int, numPlayers> playerColors {{31, 32, 33, 34}};

constexpr int defaultColor = 0;
constexpr int monitorColor = 46;

std::string setColor(int background, int foreground);

inline std::string clearColor() { return "\e[0m"; }
inline std::string clearScreen() { return "\e[2J"; }

inline
bool canMove(int fieldType, std::size_t direction) {
    constexpr std::array<int, numNeighbors> directionBits{{
            2, 1, 8, 4}};
    assert(direction <  numNeighbors);
    return fieldType & directionBits[direction];
}

const std::vector<int>& getIsomorphs(int fieldType);

constexpr int BOXHEIGHT = 5;
constexpr int BOXWIDTH = 9;
std::string getBoxLine(const Field& field, unsigned i);
std::string toBox(const Field& field);

#endif // SEMIFINAL_TRACK_FIELD_HPP
