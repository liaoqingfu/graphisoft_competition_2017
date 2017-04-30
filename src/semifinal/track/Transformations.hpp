#ifndef SEMIFINAL_TRACK_TRANSFORMATIONS_HPP
#define SEMIFINAL_TRACK_TRANSFORMATIONS_HPP

#include "Track.hpp"

#include <type_traits>
#include <vector>

namespace detail {

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

template<typename Container>
void rotatePoints(Container& points, int Point::*toCheck,
        int Point::*toMove, int position, int delta, int size) {
    rotatePoints(points, toCheck, toMove, position, delta, size,
            [](Point& original, Point p) { original = p; });
}

} // namespace detail

template<typename Container>
void transformPoints(
        std::size_t width, std::size_t height,
        Container& points, Directions direction, int position) {
    Point delta = neighbors[direction];
    auto action = [](Point& original, Point p) { original = p; };
    if (delta.y == 0) {
        detail::rotatePoints(points, &Point::y, &Point::x, position, delta.x,
                width, action);
    } else {
        assert(delta.x == 0);
        detail::rotatePoints(points, &Point::x, &Point::y, position, delta.y,
                height, action);
    }
}

template<typename Container>
void transformPoints(const Track& track,
        Container& points, Directions direction, int position) {
    return transformPoints(track.width(), track.height(),
            points, direction, position);
}

#endif // SEMIFINAL_TRACK_TRANSFORMATIONS_HPP
