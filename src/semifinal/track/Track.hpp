#ifndef SEMIFINAL_TRACK_TRACK_HPP
#define SEMIFINAL_TRACK_TRACK_HPP

#include "Field.hpp"
#include "Matrix.hpp"

#include <iostream>

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

    // Returns the field that is pushed out.
    int moveFields(int direction, int position, int fieldToPush); // TODO

    bool canMovePrincess(int player, Point target) const; // TODO
    void movePrincess(int player, Point target);
    void removeMonitor(int id);

private:
    Matrix<Field> fields;
    Monitors monitors;
    Princesses princesses;
};

#endif // SEMIFINAL_TRACK_TRACK_HPP
