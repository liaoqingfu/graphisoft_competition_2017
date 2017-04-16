#ifndef SEMIFINAL_TRACK_TRACK_HPP
#define SEMIFINAL_TRACK_TRACK_HPP

#include "Field.hpp"
#include "Matrix.hpp"

#include <iostream>
#include <memory>

class Track {
public:
    using Monitors = std::vector<Point>;
    using Princesses = std::vector<Point>;

    Track(std::size_t width, std::size_t height,
            const std::vector<int>& fieldTypes,
            Monitors monitors, Princesses princesses);

    Track(const Track&) = default;
    Track(Track&&) = default;
    Track& operator=(const Track&) = default;
    Track& operator=(Track&&) = default;

    bool operator==(const Track& other) const {
        return fields == other.fields;
    }

    std::size_t width() const { return fields.width(); }
    std::size_t height() const { return fields.height(); }
    const Field& getField(Point p) const { return fields[p]; }
    Point getMonitor(int id) const { return monitors[id]; }
    Point getPrincess(int player) const { return princesses[player]; }

    // Returns the field that is pushed out.
    int moveFields(std::size_t direction, int position, int fieldToPush);

    bool canMovePrincess(int player, Point target) const;
    void movePrincess(int player, Point target);
    void removeMonitor(int id);

private:
    void calculateReachability(Point from) const;
    void resetReachability();

    Matrix<Field> fields;
    Monitors monitors;
    Princesses princesses;
    mutable Matrix<int> reachability;
    mutable int reachabilityIndex = 0;
};

std::ostream& operator<<(std::ostream& os, const Track& track);

#endif // SEMIFINAL_TRACK_TRACK_HPP
