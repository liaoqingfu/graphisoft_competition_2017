#ifndef LABIRINTH_FIELD_HPP
#define LABIRINTH_FIELD_HPP

#include "Matrix.hpp"
#include "MatrixIO.hpp"

enum class Field : char {
    wall = 'W', corridor = 'C', monitor = 'M'
};

std::istream& operator>>(std::istream& is, Field& f) {
    char c;
    is >> c;
    f = static_cast<Field>(c);
    return is;
}

std::string to_string(Field node) {
    return std::string{static_cast<char>(node)};
}

bool isPassable(Field field) {
    return field != Field::wall;
}

#endif // LABIRINTH_FIELD_HPP
