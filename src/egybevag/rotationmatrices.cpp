#include <array>
#include <cassert>
//#include <cmath>
#include <iostream>
#include <set>

using RotationMatrix = std::array<std::array<int, 3>, 3>;

int s(int deg) {
    assert(deg < 360 && deg % 90 == 0);
    if (deg == 90) return 1;
    else if (deg == 180 || deg == 0) return 0;
    else if (deg == 270) return -1;
    else assert(0);
}

int c(int deg) {
    assert(deg < 360 && deg % 90 == 0);
    if (deg == 90 || deg == 270) return 0;
    else if (deg == 180) return -1;
    else if (deg == 0) return 1;
    else assert(0);
}

int main() {
    std::set<RotationMatrix> matrices;
    for (int u = 0; u < 360; u += 90) {
        for (int v = 0; v < 360; v += 90) {
            for (int w = 0; w < 360; w += 90) {
                //std::cout << "roll: " << u << " pitch: " << u << " yaw: " << w
                //          << std::endl;
                RotationMatrix r = {{
                    {{c(v)*c(w), s(u)*s(v)*c(w)-c(u)*s(w), s(u)*s(w)+c(u)*s(v)*c(w)}},
                    {{c(v)*s(w), c(u)*c(w)+s(u)*s(v)*s(w), c(u)*s(v)*s(w)-s(u)*c(w)}},
                    {{-s(v)    , s(u)*c(v)               , c(u)*c(v)               }}
                }};
                matrices.insert(r);
            }
        }
    }
    std::cout << "// Number of rotation matrices: " <<  matrices.size()
              << std::endl;
    std::cout << "using RotationMatrix = std::array<std::array<int, 3>, 3>;"
              << std::endl;
    std::cout << "const std::vector<RotationMatrix> rotations = {"
              << std::endl;
    for (const auto& r : matrices) {
        std::cout << "    {{" << std::endl;
        for (int i = 0; i < 3; ++i) {
            std::cout << "        {{" << r[i][0] << ", " << r[i][1] << ", "
                      << r[i][2] << "}}," << std::endl;
        }
        std::cout << "    }}," << std::endl;
    }
    std::cout << "};" << std::endl;
}
