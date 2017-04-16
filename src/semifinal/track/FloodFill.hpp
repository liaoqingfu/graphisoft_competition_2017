#ifndef SEMIFINAL_TRACK_FLOODFILL_HPP
#define SEMIFINAL_TRACK_FLOODFILL_HPP

#include "Matrix.hpp"
#include "Field.hpp"

template <typename Action>
void floodFill(const Matrix<Field>& matrix, Point p0, Action action)
{
    Matrix<bool> reached{matrix.width(), matrix.height(), false};

    std::vector<Point> pointsToVisit;
    pointsToVisit.reserve(matrix.width()*matrix.height());
    pointsToVisit.push_back(p0);

    while (!pointsToVisit.empty()) {
        Point p = pointsToVisit.back();
        pointsToVisit.pop_back();

        if (matrixAt(reached, p, true)) {
            continue;
        }
        action(p);

        reached[p] = true;
        for (std::size_t direction = 0; direction < numNeighbors; ++direction) {
            Point target = p + neighbors[direction];
            if (!isInsideMatrix(matrix, target)) {
                continue;
            }
            if (canMove(matrix[p].type, direction) && canMove(
                    matrix[target].type, oppositeDirection(direction))) {
                pointsToVisit.push_back(target);
            }
        }
    }
}

#endif // SEMIFINAL_TRACK_FLOODFILL_HPP
