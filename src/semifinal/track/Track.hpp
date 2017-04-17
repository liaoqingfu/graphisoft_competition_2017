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
    std::size_t getRemainingMonitors() const { return remainingMonitors; }
    Point getPrincess(int player) const { return princesses[player]; }

    // Returns the field that is pushed out.
    int moveFields(std::size_t direction, int position, int fieldToPush);

    bool canMovePrincess(int player, Point target) const;
    bool isReachableFrom(Point source, Point target) const;
    const std::vector<Point>& getReachablePoints(Point source) const;

    void movePrincess(int player, Point target);
    void removeMonitor(int id);

private:
    struct ReachabilityClass {
        std::vector<Point> elements;
    };

    void calculateReachability(Point from) const;
    void resetReachability();

    Matrix<Field> fields;
    Monitors monitors;
    std::size_t remainingMonitors;
    Princesses princesses;
    mutable Matrix<std::shared_ptr<ReachabilityClass>> reachability;
};

std::ostream& operator<<(std::ostream& os, const Track& track);

struct TransformedPoint {
    Point original;
    Point transformed;
};

class TransformedPointCompare {
public:
    TransformedPointCompare(Point TransformedPoint::* fieldToCompare) :
            fieldToCompare(fieldToCompare) {
    }

    bool operator()(const TransformedPoint& lhs, const TransformedPoint& rhs) {
        return lhs.*fieldToCompare < rhs.*fieldToCompare;
    }

    bool operator()(Point lhs, const TransformedPoint& rhs) {
        return lhs < rhs.*fieldToCompare;
    }

    bool operator()(const TransformedPoint& lhs, Point rhs) {
        return lhs.*fieldToCompare < rhs;
    }

private:
    Point TransformedPoint::* fieldToCompare;
};

std::vector<TransformedPoint> transformPoints(const Track& track,
        const std::vector<Point>& points, Directions direction, int position);

#endif // SEMIFINAL_TRACK_TRACK_HPP
