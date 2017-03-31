#ifndef TOOLS_MATRIX_HEXMATRIX_HPP
#define TOOLS_MATRIX_HEXMATRIX_HPP

#include "Matrix.hpp"

#include <istream>
#include <ostream>

namespace hex {

template<typename T>
void printMatrix(std::ostream& os, const Matrix<T>& matrix) {
    os << "  ";
    for (int x = 0; x < static_cast<int>(matrix.width()); ++x) {
        os << x % 10 << " ";
    }
    os << "\n";
    for (int y = 0; y < static_cast<int>(2*matrix.height()); ++y) {
        if (y % 2 == 0) {
            os << (y / 2) % 10 << " ";
        } else {
            os << "  ";
        }
        for (int x = 0; x < static_cast<int>(matrix.width()); ++x) {
            if (x % 2 == y % 2) {
                os << to_string(matrix[Point{x, y / 2}]) << " ";
            } else {
                os << "  ";
            }
        }
        os << "\n";
    }
}

constexpr std::size_t numNeighbors = 6;
using Neighbors = std::array<Point, numNeighbors>;

constexpr Neighbors oddNeighbors{
        {-px, -p10, -p01, p10, p11, p01}};

constexpr Neighbors evenNeighbors{
        {-p10, -p11, -p01, px, p10, p01}};

constexpr const Neighbors& getNeighbors(Point p) {
    return p.x % 2 == 0 ? evenNeighbors : oddNeighbors;
}

inline
Point local2ecosim(Point p, int halfWidth) {
    Point result{p.x / 2 + 1, p.y + 1};
    if (p.x % 2 != 0) {
        result.x += halfWidth;
    }
    return result;
}

namespace detail {

template<typename T>
void readElement(std::istream& stream, Matrix<T>& matrix, Point p) {
    auto& field = matrix[p];
    stream >> field;
}

}

template<typename T>
Matrix<T> readEcosim(std::istream& stream) {
    int k, n;
    stream >> k >> n;
    Matrix<T> matrix;
    matrix.reset(2*n, k);
    for (int y = 0; y < k; ++y) {
        for (int x = 0; x < n; ++x) {
            detail::readElement(stream, matrix, Point{2*x, y});
        }
        for (int x = 0; x < n; ++x) {
            detail::readElement(stream, matrix, Point{2*x + 1, y});
        }
    }
    return matrix;
}

template<typename T>
void printEcosim(std::ostream& stream, const Matrix<T> matrix) {
    int n = matrix.width() / 2;
    int k = matrix.height();
    stream << k << " " << n << "\n";

    for (int y = 0; y < k; ++y) {
        for (int x = 0; x < n; ++x) {
            stream << matrix[Point{2*x, y}] << " ";
        }
        for (int x = 0; x < n; ++x) {
            stream << matrix[Point{2*x + 1, y}] << " ";
        }
        stream << "\n";
    }
}

}

#endif // TOOLS_MATRIX_HEXMATRIX_HPP
