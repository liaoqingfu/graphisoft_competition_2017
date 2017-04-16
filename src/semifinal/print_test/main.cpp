#include "Track.hpp"

#include <iostream>

int main() {
    Track track{7, 7, {
            12, 10, 10, 14, 10, 10,  6,
             5,  4,  8, 15,  2,  4,  5,
             5,  5,  4,  5,  4,  5,  5,
            13, 15, 15, 15, 15, 15,  7,
             5,  5,  1,  5,  1,  5,  5,
             5,  1,  8, 15,  2,  1,  5,
             9, 10, 10, 11, 10, 10,  3},
          {Point{3, 3}, Point{0, 0}},
          {Point{0, 0}, Point{6, 0}, Point{0, 6}, Point{6, 6}}};
    std::cout << track;
}
