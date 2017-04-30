#include "Track.hpp"

#include "FloodFill.hpp"

#include <algorithm>

Track::Track(std::size_t width, std::size_t height,
        const std::vector<int>& fieldTypes,
        Monitors monitors, Princesses princesses) :
        fields{width, height},
        monitors(std::move(monitors)),
        remainingMonitors(this->monitors.size()),
        princesses(std::move(princesses)) {
    std::transform(fieldTypes.begin(), fieldTypes.end(), fields.begin(),
            [](int type) { return Field{type, -1}; });
    for (std::size_t i = 0; i < this->monitors.size(); ++i) {
        if (isInsideMatrix(fields, this->monitors[i])) {
            fields[this->monitors[i]].monitor = i;
        }
    }
    for (std::size_t i = 0; i < this->princesses.size(); ++i) {
        if (isInsideMatrix(fields, this->princesses[i])) {
            fields[this->princesses[i]].addPrincess(i);
        }
    }
}

bool Track::isReachableFrom(Point source, Point target) const {
    return floodFill(fields, source, [](Point){})[target];
}

std::vector<Point> Track::getReachablePoints(Point source) const {
    std::vector<Point> reachability;
    floodFill(fields, source,
            [&reachability](Point p) {
                reachability.push_back(p);
            });
    return reachability;
}

bool Track::canMovePrincess(int player, Point target) const {
    return isReachableFrom(princesses[player], target);
}

std::vector<int> Track::canMoveAnyPrincess(Point target) const {
    std::vector<int> result;
    int player = 0;
    for (auto p : this->princesses) {
        if (isReachableFrom(p, target)) result.push_back(player);
        ++player;
    }
    return result;
}

void Track::movePrincess(int player, Point target) {
    auto& from = fields[princesses[player]];
    auto& to = fields[target];

    assert(from.hasPrincess(player));

    from.removePrincess(player);
    to.addPrincess(player);

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

template<typename Container, typename ToCheck, typename ToMove, typename Action>
void rotatePoints(Container& points, const ToCheck& toCheck,
        const ToMove& toMove, int position, int delta, int size,
        const Action& action) {
    for (auto& original : points) {
        auto p = original;
        if (toCheck(p) == position) {
            toMove(p) += delta;
            if (toMove(p) < 0) {
                toMove(p) = size - 1;
            }
            if (toMove(p) >= size) {
                toMove(p) = 0;
            }
        }
        action(original, p);
    }
}

template<typename Container, typename Action>
void rotatePoints(Container& points, int Point::*toCheck,
        int Point::*toMove, int position, int delta, int size,
        const Action& action) {
    return rotatePoints(points,
            [toCheck](Point& p) -> int& { return p.*toCheck; },
            [toMove](Point& p) -> int& { return p.*toMove; },
            position, delta, size, action);
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

std::set<int> Track::getAliveMonitors() const {
    std::set<int> result;
    for (const auto& m : monitors) {
        if (m.x >= 0) {
            assert(getField(m).monitor != -1);
            result.insert(getField(m).monitor);
        }
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const Track& track) {
    os << toBox(track);
    return os;
}

namespace {

std::tuple<bool, int> getColor(Point p, const Track& track,
                                       int currentPrincess, int targetMonitor) {
    (void)targetMonitor;

    auto getColorId = [currentPrincess](int playerId) {
        int colorId = 0;
        std::string color;
        colorId = playerColors[playerId];
        //color = setColor(defaultColor, colorId);
        return colorId;
    };

    bool drawColor = currentPrincess >= 0;

    // those players who can step into this field
    std::vector<int> players = track.canMoveAnyPrincess(p);

    int colorId = 0;

    // Favorize the color of the current player
    auto it = std::find(players.begin(), players.end(), currentPrincess);
    if (it != players.end()) {
        colorId = getColorId(*it);
    } else if (!players.empty()) {
        colorId = getColorId(players[0]);
    } else {
        drawColor = false;
    }

    return std::make_tuple(drawColor, colorId);
}

} // unnamed namespace

std::string toBox(const Track& track, int currentPrincess, int targetMonitor) {
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

    ColorInfo colorInfo;
    for (int color : playerColors) {
        colorInfo.princessColors.push_back(setColor(defaultColor, color));
    }

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

            int colorId = 0;
            bool drawColor = false;
            std::tie(drawColor, colorId) =
                    getColor(p, track, currentPrincess, targetMonitor);
            std::string color = setColor(defaultColor, colorId);

            {
                std::unique_ptr<ColorDrawer> colorizer;
                if (drawColor) {
                    colorizer = std::make_unique<ColorDrawer>(line, color);
                    colorInfo.baseColor = color;
                } else {
                    colorInfo.baseColor = clearColor();
                }
                if (targetMonitor >= 0
                        && track.getMonitor(targetMonitor) == p) {
                    colorInfo.monitorColor = setColor(
                            defaultColor, monitorColor);
                } else {
                    colorInfo.monitorColor = clearColor();
                }

                line.append(getBoxLine(
                    track.getField(p), y % BOXHEIGHT, colorInfo));
            }

            // additional horizontal connection
            Point next{p.x + 1, p.y};
            if ((y % BOXHEIGHT == 1 || y % BOXHEIGHT == 3) &&
                next.x < (int)track.width() &&
                (track.getField(p).type & 0b1000) >> 3 &&
                (track.getField(next).type & 0b0010) >> 1) {
                auto colorizer = drawColor
                        ? std::make_unique<ColorDrawer>(line, color)
                        : nullptr;
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

std::vector<TransformedPoint> transformPoints(
        std::size_t width, std::size_t height,
        const std::vector<Point>& points, Directions direction, int position) {
    Point delta = neighbors[direction];
    std::vector<TransformedPoint> result;
    auto action =
            [&result](Point original, Point p) {
                result.push_back(TransformedPoint{original, p});
            };
    if (delta.y == 0) {
        rotatePoints(points, &Point::y, &Point::x, position, delta.x,
                width, action);
    } else {
        assert(delta.x == 0);
        rotatePoints(points, &Point::x, &Point::y, position, delta.y,
                height, action);
    }
    return result;
}

std::vector<TransformedPoint> transformPoints(
        std::size_t width, std::size_t height,
        const std::vector<TransformedPoint>& points,
        Directions direction, int position) {
    Point delta = neighbors[direction];
    std::vector<TransformedPoint> result;
    auto action =
            [&result](const TransformedPoint& original,
                    const TransformedPoint& p) {
                result.push_back(TransformedPoint{original.original,
                        p.transformed});
            };
    auto getX = [](TransformedPoint& p) -> int& { return p.transformed.x; };
    auto getY = [](TransformedPoint& p) -> int& { return p.transformed.y; };
    if (delta.y == 0) {
        rotatePoints(points, getY, getX, position, delta.x, width, action);
    } else {
        assert(delta.x == 0);
        rotatePoints(points, getX, getY, position, delta.y, height, action);
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
