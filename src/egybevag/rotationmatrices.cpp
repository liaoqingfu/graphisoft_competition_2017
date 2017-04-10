#include <array>
#include <cassert>
//#include <cmath>
#include <iostream>
#include <set>
#include <vector>

using RotationMatrix = std::array<std::array<int, 3>, 3>;

RotationMatrix operator*(RotationMatrix lhs, RotationMatrix rhs) {
    // ABij = SUM(Aik * Bkj)
    return {{
            {{
                lhs[0][0] * rhs[0][0] + lhs[0][1] * rhs[1][0] + lhs[0][2] * rhs[2][0],
                lhs[0][0] * rhs[0][1] + lhs[0][1] * rhs[1][1] + lhs[0][2] * rhs[2][1],
                lhs[0][0] * rhs[0][2] + lhs[0][1] * rhs[1][2] + lhs[0][2] * rhs[2][2]
            }},
            {{
                lhs[1][0] * rhs[0][0] + lhs[1][1] * rhs[1][0] + lhs[1][2] * rhs[2][0],
                lhs[1][0] * rhs[0][1] + lhs[1][1] * rhs[1][1] + lhs[1][2] * rhs[2][1],
                lhs[1][0] * rhs[0][2] + lhs[1][1] * rhs[1][2] + lhs[1][2] * rhs[2][2]
            }},
            {{
                lhs[2][0] * rhs[0][0] + lhs[2][1] * rhs[1][0] + lhs[2][2] * rhs[2][0],
                lhs[2][0] * rhs[0][1] + lhs[2][1] * rhs[1][1] + lhs[2][2] * rhs[2][1],
                lhs[2][0] * rhs[0][2] + lhs[2][1] * rhs[1][2] + lhs[2][2] * rhs[2][2]
            }},
        }};
}

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

RotationMatrix createRotationMatrix(int u, int v, int w) {
    return {{
            {{c(v)*c(w), s(u)*s(v)*c(w)-c(u)*s(w), s(u)*s(w)+c(u)*s(v)*c(w)}},
            {{c(v)*s(w), c(u)*c(w)+s(u)*s(v)*s(w), c(u)*s(v)*s(w)-s(u)*c(w)}},
            {{-s(v)    , s(u)*c(v)               , c(u)*c(v)               }}
        }};
}

int main() {
    std::set<RotationMatrix> matrices;
    for (int u = 0; u < 360; u += 90) {
        for (int v = 0; v < 360; v += 90) {
            for (int w = 0; w < 360; w += 90) {
                //std::cout << "roll: " << u << " pitch: " << u << " yaw: " << w
                //          << std::endl;
                RotationMatrix r = createRotationMatrix(u, v, w);
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
// A little hacking follows
    std::vector<RotationMatrix> rot90 = {{
            createRotationMatrix(90, 0, 0),
            createRotationMatrix(0, 90, 0),
            createRotationMatrix(0, 0, 90)}};
    for (const auto& r : rot90) {
        std::cout << "    {{" << std::endl;
        for (int i = 0; i < 3; ++i) {
            std::cout << "        {{" << r[i][0] << ", " << r[i][1] << ", "
                      << r[i][2] << "}}," << std::endl;
        }
        std::cout << "    }}," << std::endl;
    }
    const RotationMatrix identity = {{ {{ 1, 0, 0 }}, {{ 0, 1, 0 }}, {{ 0, 0, 1 }} }};
    RotationMatrix r = identity;
    std::set<RotationMatrix> rr;
    rr.insert(identity);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                r = r * rot90[2];
                rr.insert(r);
            }
            r = r * rot90[1];
            rr.insert(r);
        }
        r = r * rot90[0];
        rr.insert(r);
    }
    assert(rr == matrices);
}
