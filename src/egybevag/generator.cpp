#include <cassert>
#include <iostream>
#include <random>
#include <set>
#include <unordered_set>

std::random_device rd;

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

using RotationMatrix = std::array<std::array<int, 3>, 3>;

RotationMatrix createRotationMatrix(int u, int v, int w) {
    return {{
            {{c(v)*c(w), s(u)*s(v)*c(w)-c(u)*s(w), s(u)*s(w)+c(u)*s(v)*c(w)}},
            {{c(v)*s(w), c(u)*c(w)+s(u)*s(v)*s(w), c(u)*s(v)*s(w)-s(u)*c(w)}},
            {{-s(v)    , s(u)*c(v)               , c(u)*c(v)               }}
        }};
}

static const RotationMatrix idRot = {{
        {{ 1, 0, 0 }},
        {{ 0, 1, 0 }},
        {{ 0, 0, 1 }}
    }};

struct Vertex {
    int x = 0, y = 0, z = 0;
};

static const Vertex zero;

struct Edge {
    int v1 = 0, v2 = 0;
};

struct Side {
    std::vector<int> edges;
    // no holes
    //std::vector<Side> holes;
};

void printVertices(std::vector<Vertex> vertices, const Vertex& offset = zero,
        const RotationMatrix& rm = idRot, int skip = 0) {
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> vDist(0, vertices.size() - 1);
    std::cout << vertices.size() << '\n';
    while (vertices.size() - skip > 0) {
        int pos = vDist(gen) % vertices.size();
        const Vertex& v = vertices[pos];
        std::cout << offset.x + rm[0][0] * v.x + rm[0][1] * v.y + rm[0][2] * v.z
                  << ' '
                  << offset.y + rm[1][0] * v.x + rm[1][1] * v.y + rm[1][2] * v.z
                  << ' '
                  << offset.x + rm[2][0] * v.x + rm[2][1] * v.y + rm[2][2] * v.z
                  << '\n';
        vertices.erase(vertices.begin() + pos);
    }
}

void printEdges(std::vector<Edge> edges, int skip = 0) {
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> ed(0, edges.size() - 1);
    std::cout << edges.size() << '\n';
    while (edges.size() - skip > 0) {
        int pos = ed(gen) % edges.size();
        const Edge& e = edges[pos];
        std::cout << e.v1 << ' ' << e.v2 << '\n';
        edges.erase(edges.begin() + pos);
    }
}

void printSides(std::vector<Side> sides, int skip = 0) {
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> sd(0, sides.size() - 1);
    std::cout << sides.size() << '\n';
    while (sides.size() - skip > 0) {
        int pos = sd(gen) % sides.size();
        Side& s = sides[pos];
        std::cout << s.edges.size() << ' ';
        std::uniform_int_distribution<> ed(0, s.edges.size() - 1);
        while (s.edges.size() > 0) {
            int epos = ed(gen) % s.edges.size();
            std::cout << s.edges[epos] << ' ';
            s.edges.erase(s.edges.begin() + epos);
        }
        std::cout << "0\n";
        sides.erase(sides.begin() + pos);
    }
}

int main(int argc, char* argv[]) {
    assert(argc == 4);
    int numVertices = atoi(argv[1]);
    int numEdges = atoi(argv[2]);
    int numSides = atoi(argv[3]);
    assert(numEdges <= numVertices);
    assert(numSides <= numEdges / 3);

    std::mt19937 gen(rd());
    std::uniform_int_distribution<> coordinateDist(1, 100000); // *10
    auto genVertex = [&gen, &coordinateDist]() {
        return Vertex{
            coordinateDist(gen), coordinateDist(gen), coordinateDist(gen)
        };
    };

    // number of houses
    std::cout << '2' << '\n';

    std::vector<Vertex> vertices;
    for (int i = 0; i < numVertices; ++i) {
        vertices.push_back(genVertex());
    }
    printVertices(vertices);

    std::uniform_int_distribution<> vertexDist(1, numVertices);
    std::vector<Edge> edges;
    for (int i = 0; i < numEdges; ++i) {
        Edge e{vertexDist(gen), vertexDist(gen)};
        edges.push_back(e);
    }
    printEdges(edges);

    std::uniform_int_distribution<> edgeDist(1, numEdges);
    std::uniform_int_distribution<> edgeNumDist(3,
            numEdges > 100 ? 100 : numEdges);
    std::vector<Side> sides;
    for (int i = 0; i < numSides; ++i) {
        int edgeNum = edgeNumDist(gen);
        std::vector<int> es;
        for (int j = 0; j < edgeNum; ++j) {
            int e = edgeDist(gen);
            es.push_back(e);
        }
        Side s{es};
        sides.push_back(s);
    }
    printSides(sides);

    std::cout << '\n';

    Vertex offset = genVertex();
    std::uniform_int_distribution<> rotDist(0, 3);
    RotationMatrix rm = createRotationMatrix(90 * rotDist(gen),
            90 * rotDist(gen), 90 * rotDist(gen));

    printVertices(vertices, offset, rm);
    printEdges(edges);
    printSides(sides);
}
