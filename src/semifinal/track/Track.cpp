#include "Track.hpp"

#include "FloodFill.hpp"

Track::Track(std::size_t width, std::size_t height,
        const std::vector<int>& fieldTypes,
        Monitors monitors, Princesses princesses) :
        fields{width, height},
        monitors(std::move(monitors)),
        princesses(std::move(princesses)),
        reachability{width, height, -1} {
    std::transform(fieldTypes.begin(), fieldTypes.end(), fields.begin(),
            [](int type) { return Field{type, -1, -1}; });
    for (std::size_t i = 0; i < this->monitors.size(); ++i) {
        fields[this->monitors[i]].monitor = i;
    }
    for (std::size_t i = 0; i < this->princesses.size(); ++i) {
        fields[this->princesses[i]].princess = i;
    }
}

bool Track::isReachableFrom(Point source, Point target) const {
    calculateReachability(source);
    return reachability[source] == reachability[target];
}

bool Track::canMovePrincess(int player, Point target) const {
    return isReachableFrom(princesses[player], target);
}

void Track::movePrincess(int player, Point target) {
    int& fromPrincess = fields[princesses[player]].princess;
    int& toPrincess = fields[target].princess;
    assert(fromPrincess == player);
    assert(toPrincess == -1);
    // canMove() is not checked: it's expensive

    fromPrincess = -1;
    toPrincess = player;
    princesses[player] = target;
}

namespace {

void calculateSwapDirection(int delta, int size, int& begin, int& end) {
    if (delta < 0) {
        begin = 0;
        end = size - 1;
    } else {
        begin = size - 1;
        end = 0;
    }
}

void rotatePoints(std::vector<Point>& points, int Point::*toCheck,
        int Point::*toMove, int position, int delta, int size) {
    for (Point& p : points) {
        if (p.*toCheck == position) {
            p.*toMove += delta;
            if (p.*toMove < 0) {
                p.*toMove = size - 1;
            }
            if (p.*toMove >= size) {
                p.*toMove = 0;
            }
        }
    }
}

}

int Track::moveFields(std::size_t direction, int position, int fieldToPush) {
    assert(direction >= 0);
    assert(direction < numNeighbors);
    Point delta = neighbors[direction];
    std::size_t swapDirection = oppositeDirection(direction);
    Point begin, end;
    if (delta.y == 0) {
        begin.y = position;
        end.y = position;
        calculateSwapDirection(delta.x, width(), begin.x, end.x);
        rotatePoints(monitors, &Point::y, &Point::x, position, delta.x,
                width());
        rotatePoints(princesses, &Point::y, &Point::x, position, delta.x,
                width());
    } else {
        assert(delta.x == 0);
        begin.x = position;
        end.x = position;
        calculateSwapDirection(delta.y, height(), begin.y, end.y);
        rotatePoints(monitors, &Point::x, &Point::y, position, delta.y,
                height());
        rotatePoints(princesses, &Point::x, &Point::y, position, delta.y,
                height());
    }

    for (Point p = begin; p != end; p += neighbors[swapDirection]) {
        std::swap(fields[p], fields[p + neighbors[swapDirection]]);
    }

    int result = fields[end].type;
    fields[end].type = fieldToPush;

    resetReachability();

    return result;
}

void Track::removeMonitor(int id) {
    int& monitor = fields[monitors[id]].monitor;
    assert(monitor == id);
    assert(isInsideMatrix(fields, monitors[id]));

    monitor = -1;
    monitors[id] = -p11;
}

void Track::calculateReachability(Point from) const {
    if (reachability[from] >= 0) {
        return;
    }
    floodFill(fields, from,
            [this](Point p) { reachability[p] = reachabilityIndex; });
    ++reachabilityIndex;
}

void Track::resetReachability() {
    reachability.fill(-1);
    reachabilityIndex = 0;
}

std::ostream& operator<<(std::ostream& os, const Track& track) {
    os << "  ";
    for (std::size_t i = 0; i < track.width(); ++i) {
        os << i % 10 << " ";
    }
    os << "\n";
    Point p;
    for (p.y = 0; p.y < static_cast<int>(track.height()); ++p.y) {
        os << p.y % 10 << " ";
        for (p.x = 0; p.x < static_cast<int>(track.width()); ++p.x) {
            if (p.x != 0) {
                os << "─";
            }
            os << track.getField(p);
        }
        os << "\n";
    }
    return os;
}
