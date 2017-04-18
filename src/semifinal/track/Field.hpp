#ifndef SEMIFINAL_TRACK_FIELD_HPP
#define SEMIFINAL_TRACK_FIELD_HPP

#include "Point.hpp"

#include <algorithm>
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

    Field() : type(0), monitor(-1), princesses() {}
    Field(int type, int monitor, std::vector<int> princesses)
        : type(type), monitor(monitor), princesses(std::move(princesses)) {}

    void removePrincess(int p) {
        princesses.erase(std::remove(princesses.begin(), princesses.end(), p),
                         princesses.end());
    }
    void addPrincess(int p) { princesses.push_back(p); }
    bool hasPrincess(int p) const {
        return std::find(princesses.begin(), princesses.end(), p) !=
               princesses.end();
    }
    bool noPrincess() const { return princesses.empty(); }
    const std::vector<int>& getPrincesses() const { return princesses; }

    friend bool operator==(const Field& lhs, const Field& rhs) {
        return lhs.type == rhs.type && lhs.monitor == rhs.monitor &&
               lhs.princesses == rhs.princesses;
    }

private:
    std::vector<int> princesses;
};

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
