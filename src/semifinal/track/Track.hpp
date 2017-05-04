#ifndef SEMIFINAL_TRACK_TRACK_HPP
#define SEMIFINAL_TRACK_TRACK_HPP

#include "Field.hpp"
#include "Matrix.hpp"

#include <iostream>
#include <memory>
#include <set>

class Track {
public:
    using Monitors = std::vector<Point>;
    using Princesses = std::vector<Point>;

    Track() = default;
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
    std::set<int> getAliveMonitors() const;
    const Monitors& getAllMonitors() const { return monitors; }
    std::size_t getRemainingMonitors() const { return remainingMonitors; }
    Point getPrincess(int player) const { return princesses[player]; }

    // Returns the field that is pushed out.
    int moveFields(std::size_t direction, int position, int fieldToPush);

    bool canMovePrincess(int player, Point target) const;
    bool isReachableFrom(Point source, Point target) const;
    std::vector<Point> getReachablePoints(Point source) const;
    // returns the playerIds who can move to target
    std::vector<int> canMoveAnyPrincess(Point target) const;

    void movePrincess(int player, Point target);
    void removeMonitor(int id);

private:

    Matrix<Field> fields;
    Monitors monitors;
    std::size_t remainingMonitors;
    Princesses princesses;
};

std::ostream& operator<<(std::ostream& os, const Track& track);

std::string toBox(const Track& track, int currentPrincess = -1,
        int targetMonitor = -1, const std::vector<Point>& blocked = {});

int getExtraField(const Track& track, Directions direction, int position);

#endif // SEMIFINAL_TRACK_TRACK_HPP
