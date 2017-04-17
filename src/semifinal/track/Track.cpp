#include "Track.hpp"

#include "FloodFill.hpp"

Track::Track(std::size_t width, std::size_t height,
        const std::vector<int>& fieldTypes,
        Monitors monitors, Princesses princesses) :
        fields{width, height},
        monitors(std::move(monitors)),
        remainingMonitors(this->monitors.size()),
        princesses(std::move(princesses)),
        reachability{width, height} {
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

const std::vector<Point>& Track::getReachablePoints(Point source) const {
    calculateReachability(source);
    return reachability[source]->elements;
}

bool Track::canMovePrincess(int player, Point target) const {
    return isReachableFrom(princesses[player], target)
            && fields[target].princess == -1;
}

void Track::movePrincess(int player, Point target) {
    int& fromPrincess = fields[princesses[player]].princess;
    int& toPrincess = fields[target].princess;
    assert(fromPrincess == player);
    assert(toPrincess == -1);
    // canMove() is not checked: it's expe

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

template<typename Container, typename Action>
void rotatePoints(Container& points, int Point::*toCheck,
        int Point::*toMove, int position, int delta, int size,
        const Action& action) {
    for (auto& original : points) {
        Point p = original;
        if (p.*toCheck == position) {
            p.*toMove += delta;
            if (p.*toMove < 0) {
                p.*toMove = size - 1;
            }
            if (p.*toMove >= size) {
                p.*toMove = 0;
            }
        }
        action(original, p);
    }
}

void rotatePoints(std::vector<Point>& points, int Point::*toCheck,
        int Point::*toMove, int position, int delta, int size) {
    rotatePoints(points, toCheck, toMove, position, delta, size,
            [](Point& original, Point p) { original = p; });
}

}

int Track::moveFields(std::size_t direction, int position, int fieldToPush) {
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
    --remainingMonitors;
}

void Track::calculateReachability(Point from) const {
    if (reachability[from]) {
        return;
    }
    auto reachabilityClass = std::make_shared<ReachabilityClass>();
    floodFill(fields, from,
            [this, &reachabilityClass](Point p) {
                reachability[p] = reachabilityClass;
                reachabilityClass->elements.push_back(p);
            });
}

void Track::resetReachability() {
    reachability.fill({});
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

std::string toBox(const Track& track) {
    std::string result;

    // header for X axis
    result.append("   "); /*Y axis offset*/
    for (std::size_t i = 0; i < track.width(); ++i) {
        auto num = std::to_string(i);
            result.append("    ")  /*to middle of field*/
            .append(num);
        for (std::size_t j = 0; j <= BOXWIDTH / 2 - num.size(); ++j) {
            result.append(" ");
        }

        // for additonal horizontal connection
        if (i < track.width() - 1) {
            result.append(" ");
        }

    }
    result.append("\n");

    Point p;
    for (unsigned int y = 0; y < track.height() * BOXHEIGHT; ++y) {
        std::string line;

        //header for Y axis
        if (y % BOXHEIGHT == BOXHEIGHT / 2) {
            auto num = std::to_string(y / BOXHEIGHT);
            line.append(num);
            for (std::size_t j = 0; j <= 2 - num.size(); ++j) {
                line.append(" ");
            }
        } else {
            line.append("   ");
        }

        p.y = y / BOXHEIGHT;
        for (p.x = 0; p.x < static_cast<int>(track.width()); ++p.x) {
            line.append(getBoxLine(track.getField(p), y % BOXHEIGHT));

            // additional horizontal connection
            Point next{p.x + 1, p.y};
            if (y % BOXHEIGHT == BOXHEIGHT / 2 && next.x < (int)track.width() &&
                (track.getField(p).type & 0b1000) >> 3 &&
                (track.getField(next).type & 0b0010) >> 1) {
                line.append("━");
            } else if (p.x < (int)track.width() -
                                 1) { // do not append space after the last col
                line.append(" ");
            }

        }
        line.append("\n");
        result.append(line);
    }
    return result;
}

std::vector<TransformedPoint> transformPoints(const Track& track,
        const std::vector<Point>& points, Directions direction, int position) {
    Point delta = neighbors[direction];
    std::vector<TransformedPoint> result;
    auto action =
            [&result](Point original, Point p) {
                result.push_back(TransformedPoint{original, p});
            };
    if (delta.y == 0) {
        rotatePoints(points, &Point::y, &Point::x, position, delta.x,
                track.width(), action);
    } else {
        assert(delta.x == 0);
        rotatePoints(points, &Point::x, &Point::y, position, delta.y,
                track.height(), action);
    }
    return result;
}

int getExtraField(const Track& track, Directions direction, int position) {
    Point delta = neighbors[direction];
    Point begin, end;
    if (delta.y == 0) {
        begin.y = position;
        calculateSwapDirection(delta.x, track.width(), begin.x, end.x);
    } else {
        assert(delta.x == 0);
        begin.x = position;
        calculateSwapDirection(delta.y, track.height(), begin.y, end.y);
    }
    return track.getField(begin).type;
}
