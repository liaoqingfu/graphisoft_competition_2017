#include "Matrix.hpp"
#include "MatrixIO.hpp"

#include <iostream>

enum class Field : char {
    wall = 'W', corridor = 'C', monitor = 'M'
};

std::istream& operator>>(std::istream& is, Field& f) {
    char c;
    is >> c;
    f = static_cast<Field>(c);
    return is;
}

struct InputNode {
    Field field;
};

std::string to_string(InputNode node) {
    return std::string{static_cast<char>(node.field)};
}

struct Input {
    Matrix<InputNode> matrix;
    std::size_t numNodes = 0;
};

void readElement(std::istream& stream, Input& input, Point p) {
    Field& field = input.matrix[p].field;
    stream >> field;
    if (field != Field::wall) {
        ++input.numNodes;
    }
}

Input readInput(std::istream& stream) {
    int k, n;
    stream >> k >> n;
    Input input;
    input.matrix.reset(2*n, k);
    for (int y = 0; y < k; ++y) {
        for (int x = 0; x < n; ++x) {
            readElement(stream, input, Point{2*x, y});
        }
        for (int x = 0; x < n; ++x) {
            readElement(stream, input, Point{2*x + 1, y});
        }
    }
    return input;
}

int main() {
    Input input = readInput(std::cin);
    std::cout << input.matrix;
}
