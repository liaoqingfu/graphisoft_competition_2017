#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

//============================================================================//

// Number of rotation matrices: 24
using RotationMatrix = std::array<std::array<int, 3>, 3>;

const RotationMatrix identity = {{ {{ 1, 0, 0, }}, {{ 0, 1, 0 }}, {{ 0, 0, 1 }} }};

const std::vector<RotationMatrix> rotations = {
    {{
        {{1, 0, 0}},
        {{0, 1, 0}},
        {{0, 0, 1}},
    }},
    {{
        {{-1, 0, 0}},
        {{0, -1, 0}},
        {{0, 0, 1}},
    }},
    {{
        {{-1, 0, 0}},
        {{0, 0, -1}},
        {{0, -1, 0}},
    }},
    {{
        {{-1, 0, 0}},
        {{0, 0, 1}},
        {{0, 1, 0}},
    }},
    {{
        {{-1, 0, 0}},
        {{0, 1, 0}},
        {{0, 0, -1}},
    }},
    {{
        {{0, -1, 0}},
        {{-1, 0, 0}},
        {{0, 0, -1}},
    }},
    {{
        {{0, -1, 0}},
        {{0, 0, -1}},
        {{1, 0, 0}},
    }},
    {{
        {{0, -1, 0}},
        {{0, 0, 1}},
        {{-1, 0, 0}},
    }},
    {{
        {{0, -1, 0}},
        {{1, 0, 0}},
        {{0, 0, 1}},
    }},
    {{
        {{0, 0, -1}},
        {{-1, 0, 0}},
        {{0, 1, 0}},
    }},
    // 11
    {{
        {{0, 0, -1}},
        {{0, -1, 0}},
        {{-1, 0, 0}},
    }},
    {{
        {{0, 0, -1}},
        {{0, 1, 0}},
        {{1, 0, 0}},
    }},
    {{
        {{0, 0, -1}},
        {{1, 0, 0}},
        {{0, -1, 0}},
    }},
    {{
        {{0, 0, 1}},
        {{-1, 0, 0}},
        {{0, -1, 0}},
    }},
    {{
        {{0, 0, 1}},
        {{0, -1, 0}},
        {{1, 0, 0}},
    }},
    {{
        {{0, 0, 1}},
        {{0, 1, 0}},
        {{-1, 0, 0}},
    }},
    {{
        {{0, 0, 1}},
        {{1, 0, 0}},
        {{0, 1, 0}},
    }},
    {{
        {{0, 1, 0}},
        {{-1, 0, 0}},
        {{0, 0, 1}},
    }},
    {{
        {{0, 1, 0}},
        {{0, 0, -1}},
        {{-1, 0, 0}},
    }},
    {{
        {{0, 1, 0}},
        {{0, 0, 1}},
        {{1, 0, 0}},
    }},
    {{
        {{0, 1, 0}},
        {{1, 0, 0}},
        {{0, 0, -1}},
    }},
    {{
        {{1, 0, 0}},
        {{0, -1, 0}},
        {{0, 0, -1}},
    }},
    {{
        {{1, 0, 0}},
        {{0, 0, -1}},
        {{0, 1, 0}},
    }},
    {{
        {{1, 0, 0}},
        {{0, 0, 1}},
        {{0, -1, 0}},
    }},
};

std::vector<RotationMatrix> localRotations = {{
        {{
            {{ 1,  0,  0}},
            {{ 0,  1,  0}},
            {{ 0,  0,  1}}
        }},
        {{
            {{-1,  0,  0}},
            {{ 0, -1,  0}},
            {{ 0,  0,  1}}
        }},
        {{
            {{ 1,  0,  0}},
            {{ 0,  0,  1}},
            {{ 0, -1,  0}}
        }},
        {{
            {{ 1,  0,  0}},
            {{ 0, -1,  0}},
            {{ 0,  0, -1}}
        }},
        {{
            {{ 1,  0,  0}},
            {{ 0,  0, -1}},
            {{ 0,  1,  0}}
        }},
        {{
            {{ 0,  1,  0}},
            {{-1,  0,  0}},
            {{ 0,  0,  1}}
        }},
        {{
            {{ 0,  0,  1}},
            {{ 0,  1,  0}},
            {{-1,  0,  0}}
        }},
        {{
            {{-1,  0,  0}},
            {{ 0,  1,  0}},
            {{ 0,  0, -1}}
        }},
        {{
            {{ 0,  0, -1}},
            {{ 0,  1,  0}},
            {{ 1,  0,  0}}
        }},
        {{
            {{-1,  0,  0}},
            {{ 0,  0,  1}},
            {{ 0,  1,  0}}
        }},
        {{
            {{ 0,  1,  0}},
            {{-1,  0,  0}},
            {{ 0,  0,  1}}
        }},
        {{
            {{-1,  0,  0}},
            {{ 0, -1,  0}},
            {{ 0,  0,  1}}
        }},
        {{
            {{ 0, -1,  0}},
            {{ 1,  0,  0}},
            {{ 0,  0,  1}}
        }},
        {{
            {{-1,  0,  0}},
            {{ 0,  1,  0}},
            {{ 0,  0, -1}}
        }},
        {{
            {{ 0,  1,  0}},
            {{-1,  0,  0}},
            {{ 0,  0,  1}}
        }},
        {{
            {{-1,  0,  0}},
            {{ 0, -1,  0}},
            {{ 0,  0,  1}}
        }},
        {{
            {{ 0, -1,  0}},
            {{ 1,  0,  0}},
            {{ 0,  0,  1}}
        }},
        {{
            {{-1,  0,  0}},
            {{ 0,  0,  1}},
            {{ 0,  1,  0}}
        }},
        {{
            {{ 0,  0,  1}},
            {{ 0,  1,  0}},
            {{-1,  0,  0}}
        }},
        {{
            {{-1,  0,  0}},
            {{ 0,  1,  0}},
            {{ 0,  0, -1}}
        }},
        {{
            {{ 0,  0, -1}},
            {{ 0,  1,  0}},
            {{ 1,  0,  0}}
        }},
        {{
            {{ 0, -1,  0}},
            {{ 1,  0,  0}},
            {{ 0,  0,  1}}
        }},
        {{
            {{ 1,  0,  0}},
            {{ 0,  0,  1}},
            {{ 0, -1,  0}}
        }},
        {{
            {{ 1,  0,  0}},
            {{ 0, -1,  0}},
            {{ 0,  0, -1}}
        }},
}};


//============================================================================//

// struct Quaternion {
//     double w = 0, x = 0, y = 0, z = 0;
// };
//
// static const double s2 = std::sqrt(2.0);
//
// static const std::vector<Quaternion> qrotations = {
//     // w,   i,   j,   k
//     { 1.,  .0,  .0,  .0},
//     { s2,  .0,  s2,  .0},
//     { .0,  .0,  1.,  .0},
//     { s2,  .0, -s2,  .0},
//
//     { s2,  .0,  .0,  s2},
//     { .5,  .5,  .5,  .5},
//     { .0,  s2,  s2,  .0},
//     { .5, -.5, -.5,  .5},
//
//     { s2,  .0,  .0, -s2},
//     { .5, -.5,  .5, -.5},
//     { .0, -s2,  s2,  .0},
//     { .5,  .5, -.5, -.5},
//
//     { s2,  s2,  .0,  .0},
//     { .5,  .5,  .5, -.5},
//     { .0,  .0,  s2, -s2},
//     { .5,  .5, -.5,  .5},
//
//     { .0,  1.,  .0,  .0},
//     { .0,  s2,  .0, -s2},
//     { .0,  .0,  .0,  1.},
//     { .0,  s2,  .0,  s2},
//
//     { s2,  s2,  .0,  .0},
//     { .5, -.5,  .5,  .5},
//     { .0,  .0,  s2,  s2},
//     { .5, -.5, -.5, -.5}
// };

//============================================================================//

template<typename T>
bool lessThanByValue(T* lhs, T* rhs) {
    return *lhs < *rhs;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

template<typename T>
struct EqualsByValue;
// template<typename T>
// struct EqualsByValue {
//     bool operator()(T* lhs, T* rhs) {
//         return *lhs == *rhs;
//     }
// };

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

struct Vertex {
    Vertex(int x, int y, int z) : x(x), y(y), z(z) {
    }

    void shift(int offX, int offY, int offZ) {
        this->x += offX;
        this->y += offY;
        this->z += offZ;
    }

    int x = 0, y = 0, z = 0, refs = 0;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

bool operator<(const Vertex& lhs, const Vertex& rhs) {
    if (lhs.x == rhs.x) {
        if (lhs.y == rhs.y) {
            if (lhs.z == rhs.z) {
                return lhs.refs < rhs.refs;
            }
            return lhs.z < rhs.z;
        }
        return lhs.y < rhs.y;
    }
    return lhs.x < rhs.x;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

bool operator==(const Vertex& lhs, const Vertex& rhs) {
    return lhs.refs == rhs.refs && lhs.x == rhs.x && lhs.y == rhs.y &&
            lhs.z == rhs.z;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

// Vertex operator-(const Vertex& lhs, const Vertex& rhs) {
//     return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
// }
//
// //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//
// Vertex operator+(const Vertex& lhs, const Vertex& rhs) {
//     return {rhs.x + lhs.x, rhs.y + lhs.y, rhs.z + lhs.z};
// }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

std::ostream& operator<<(std::ostream& out, const Vertex& v) {
    return out << "v(" << v.x << ',' << v.y << ',' << v.z << '|' << v.refs
               << ')';
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

std::istream& operator>>(std::istream& in, Vertex& v) {
    return in >> v.x >> v.y >> v.z;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

template<>
struct EqualsByValue<Vertex> {
    explicit EqualsByValue(const Vertex& offset) : offset(offset) {
    }

    bool operator()(Vertex* lhs, Vertex* rhs) {
        //std::cerr << "comparing " << *lhs << " with " << *rhs
        //          << " using offset " << offset << std::endl;
        return lhs->x - offset.x == rhs->x &&
               lhs->y - offset.y == rhs->y &&
               lhs->z - offset.z == rhs->z &&
               lhs ->refs == rhs->refs;
    }

    Vertex offset;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

bool equalsByRefs(Vertex* v1, Vertex* v2) {
    return v1->refs == v2->refs;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

struct Edge {
    Edge(Vertex* v1, Vertex* v2) : v1(v1), v2(v2) {
        ++v1->refs;
        ++v2->refs;
        sort();
    }

    void sort() {
        if (*v2 < *v1) {
            std::swap(this->v1, this->v2);
        }
    }

    Vertex* v1, * v2;
    int refs = 0;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

bool operator<(const Edge& lhs, const Edge& rhs) {
    if (*lhs.v1 == *rhs.v1) {
        if (*lhs.v2 == *rhs.v2) {
            return lhs.refs < rhs.refs;
        }
        return *lhs.v2 < *rhs.v2;
    }
    return *lhs.v1 < *rhs.v1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

bool operator==(const Edge& lhs, const Edge& rhs) {
    return lhs.refs == rhs.refs && *lhs.v1 == *rhs.v1 && *lhs.v2 == *rhs.v2;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

std::ostream& operator<<(std::ostream& out, const Edge& e) {
    return out << "e(" << *e.v1 << ',' << * e.v2 << '|' << e.refs << ')';
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

template<>
struct EqualsByValue<Edge> {
    explicit EqualsByValue(const Vertex& offset) : comparator(offset) {
    }

    bool operator()(Edge* lhs, Edge* rhs) {
        return comparator(lhs->v1, rhs->v1) &&
               comparator(lhs->v2, rhs->v2) &&
               lhs->refs == rhs->refs;
    }

    EqualsByValue<Vertex> comparator;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

struct Side {
    std::vector<Edge*> edges;
    std::vector<Side> holes;

    void sort() {
        std::sort(edges.begin(), edges.end(), lessThanByValue<Edge>);
        for (auto& hole : holes) {
            hole.sort();
        }
        std::sort(holes.begin(), holes.end());
    }

    void addEdge(Edge* edge) {
        edges.push_back(edge);
        ++edge->refs;
    }
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

std::ostream& operator<<(std::ostream& out, const Side& side) {
    out << "s[";
    for (const Edge* edge : side.edges) {
        out << *edge << ", ";
    }
    out << "h[";
    for (const Side& hole : side.holes) {
        out << hole << ", ";
    }
    return out << "]]";
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

bool operator<(const Side& lhs, const Side& rhs) {
    //std::cout << "comparing " << std::endl << lhs << std::endl << rhs << std::endl;
    bool edgesLess = std::lexicographical_compare(
            lhs.edges.begin(), lhs.edges.end(),
            rhs.edges.begin(), rhs.edges.end(),
            lessThanByValue<Edge>);
    if (lhs.holes.size() == 0 && rhs.holes.size() == 0) {
        //std::cout << "less1! " << edgesLess << std::endl;
        return edgesLess;
    }
    bool holesLess = std::lexicographical_compare(
                lhs.holes.begin(), lhs.holes.end(),
                rhs.holes.begin(), rhs.holes.end());
    return edgesLess && holesLess;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

struct SideEquals {
    explicit SideEquals(const Vertex& offset) : offset(offset) {
    }

    bool operator()(const Side& lhs, const Side& rhs) const {
        if (lhs.edges.size() != rhs.edges.size() ||
                lhs.holes.size() != rhs.holes.size()) {
            return false;
        }
        bool equalEdges = std::equal(lhs.edges.begin(), lhs.edges.end(),
                        rhs.edges.begin(), rhs.edges.end(),
                        EqualsByValue<Edge>(offset));
        if (!equalEdges) {
            return false;
        }
        bool equalHoles = std::equal(lhs.holes.begin(), lhs.holes.end(),
                        rhs.holes.begin(), rhs.holes.end(), SideEquals(offset));
        return equalHoles;
    }

    const Vertex offset;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

class Building {
private:
    std::vector<Vertex*> vertices;
    std::vector<Edge*> edges;
    std::vector<Side> sides;
    bool isSorted = false;

    void printVertices() {
        for (Vertex* v : vertices) {
            std::cout << *v << std::endl;
        }
    }

    void addVertex(int x, int y, int z) {
        assert(!isSorted);
        vertices.push_back(new Vertex(x, y, z));
    }

    void addEdge(std::size_t v1, std::size_t v2) {
        assert(!isSorted);
        assert(v1 < vertices.size());
        assert(v2 < vertices.size());
        edges.push_back(new Edge(vertices[v1], vertices[v2]));
    }

    void addSide(Side s) {
        assert(!isSorted);
        sides.push_back(std::move(s));
    }

    void copy(const Building& other) {
        if (this == &other) {
            return;
        }
        assert(vertices.empty());
        assert(edges.empty());
        assert(sides.empty());
        std::unordered_map<const Vertex*, Vertex*> vTranslationTable;
        for (const Vertex* const& v : other.vertices) {
            Vertex* vertex = new Vertex(v->x, v->y, v->z);
            vertices.push_back(vertex);
            vTranslationTable[v] = vertex;
        }
        std::unordered_map<const Edge*, Edge*> eTranslationTable;
        //std::cerr << "edges at copy: " << std::endl;
        for (const Edge* const& e : other.edges) {
            Edge* edge = new Edge(vTranslationTable[e->v1],
                    vTranslationTable[e->v2]);
            edges.push_back(edge);
            //std::cerr << edge << ' ' <<  edge->v1->refs << ' ' << edge->v2->refs
            //          << std::endl;
            eTranslationTable[e] = edge;
        }
        for (const Side& s : other.sides) {
            Side side;
            for (const Edge* e : s.edges) {
                side.addEdge(eTranslationTable[e]);
            }
            for (const Side& h : s.holes) {
                Side hole;
                for (const Edge* he : h.edges) {
                    hole.addEdge(eTranslationTable[he]);
                }
                side.holes.push_back(hole);
            }
            sides.push_back(side);
        }
        //std::cerr << "edges after copy: " << std::endl;
        //for (const auto& edge : edges) {
        //    std::cerr << edge << ' ' <<  edge->v1->refs << ' ' << edge->v2->refs
        //              << std::endl;
        //}
    }

public:
    Building() = default;

//     Building(const Building& other) {
//         //std::cout << "copycon" << std::endl;
//         copy(other);
//     }
    Building(const Building&) = delete;

    Building(Building&&) = delete;

//     Building& operator=(const Building& other) {
//         //std::cout << "opying" << std::endl;
//         copy(other);
//         return *this;
//     }
    Building& operator=(const Building) = delete;

    Building& operator=(Building&&) = delete;

    ~Building() {
        for (Vertex*& v : vertices) {
            delete v;
        }
        for (Edge*& e : edges) {
            delete e;
        }
    }

    void sort() {
        std::sort(vertices.begin(), vertices.end(), lessThanByValue<Vertex>);
        std::sort(edges.begin(), edges.end(), lessThanByValue<Edge>);
        for (auto& edge : edges) {
            edge->sort();
        }
        for (auto& side : sides) {
            side.sort();
        }
        std::sort(sides.begin(), sides.end());
        isSorted = true;
    }

    Vertex calculateOffset(const Building& rhs) const {
        return {vertices[0]->x - rhs.vertices[0]->x,
                    vertices[0]->y - rhs.vertices[0]->y,
                    vertices[0]->z - rhs.vertices[0]->z};
    }

    void shift(const Vertex& offset) {
        //std::cout << "shifting with: " << offset << std::endl;
        for (Vertex*& v : vertices) {
            //*v = *v + offset;
            v->shift(offset.x, offset.y, offset.z);
        }
    }

    void rotate(const RotationMatrix& r) {
        if (r == identity) {
            return;
        }
        for (Vertex*& v : vertices) {
            Vertex o = *v;
            v->x = r[0][0] * o.x + r[0][1] * o.y + r[0][2] * o.z;
            v->y = r[1][0] * o.x + r[1][1] * o.y + r[1][2] * o.z;
            v->z = r[2][0] * o.x + r[2][1] * o.y + r[2][2] * o.z;
//            Quaternion qo{0, static_cast<double>(o.x), static_cast<double>(o.y),
//                        static_cast<double>(o.z)};
//            o.x = 0;
        }
        //sort();
    }

    bool operator==(const Building& rhs) const {
        assert(isSorted);
        if (vertices.size() != rhs.vertices.size() ||
                edges.size() != rhs.edges.size() ||
                sides.size() != rhs.sides.size()) {
            return false;
        }
        Vertex offset = {0, 0, 0};
        if (vertices.size() > 0 && rhs.vertices.size() > 0) {
            offset = calculateOffset(rhs);
            bool verticesEqual = std::equal(vertices.begin(), vertices.end(),
                    rhs.vertices.begin(), rhs.vertices.end(),
                    EqualsByValue<Vertex>(offset));
            if (!verticesEqual) {
                return false;
            }
            // bool verticeRefsEqual = std::equal(vertices.begin(), vertices.end(),
            //         rhs.vertices.begin(), rhs.vertices.end(),
            //         equalsByRefs);
            // if (!verticeRefsEqual) {
            //     return false;
            // }
        }
        bool edgesEqual = std::equal(edges.begin(), edges.end(),
                rhs.edges.begin(), rhs.edges.end(),
                EqualsByValue<Edge>(offset));
        if (!edgesEqual) {
            return false;
        }
        bool sidesEqual = std::equal(sides.begin(), sides.end(),
                rhs.sides.begin(), rhs.sides.end(), SideEquals(offset));
        if (!sidesEqual) {
            return false;
        }
        return true;
    }

    friend std::istream& operator>>(std::istream& in, Building& b) {
        int numberOfVertices = 0;
        in >> numberOfVertices;
        //std::cout << numberOfVertices << std::endl;
        while (numberOfVertices-- > 0) {
            int x, y, z;
            in >> x >> y >> z;
            //std::cout << x << ' ' << y << ' ' << z << std::endl;
            b.addVertex(x, y, z);
        }
        int numberOfEdges = 0;
        in >> numberOfEdges;
        //std::cout << numberOfEdges << std::endl;;
        while (numberOfEdges-- > 0) {
            std::size_t v1, v2;
            in >> v1 >> v2;
            //std::cout << v1 << ' ' << v2 << std::endl;;
            b.addEdge(v1 - 1, v2 - 1); // 1-based indices
        }
        int numberOfSides = 0;
        in >> numberOfSides;
        while (numberOfSides-- > 0) {
            Side side;
            std::size_t numberOfSideEdges = 0;
            in >> numberOfSideEdges;
            assert(numberOfSideEdges <= b.edges.size());
            while (numberOfSideEdges-- > 0) {
                int edge = 0;
                in >> edge;
                side.addEdge(b.edges[edge - 1]); // 1-based
            }
            // I assume there can be no side with 0 edges
            int numberOfHoles = 0;
            in >> numberOfHoles;
            while (numberOfHoles-- > 0) {
                Side hole;
                std::size_t numberOfHoleEdges = 0;
                in >> numberOfHoleEdges;
                assert(numberOfHoleEdges <= b.edges.size());
                while (numberOfHoleEdges-- > 0) {
                    int edge = 0;
                    in >> edge;
                    hole.addEdge(b.edges[edge - 1]); // 1-based
                }
                side.holes.push_back(hole);
            }
            b.addSide(side);
        }
        //b.sort();
        return in;
    }

    friend std::ostream& operator<<(std::ostream& out, const Building& b) {
        out << "Vertices:" << std::endl;
        for (const Vertex* v : b.vertices) {
            out << *v << ", ";
        }
        out << std::endl << "Edges:" << std::endl;
        for (const Edge* e : b.edges) {
            out << *e << ", ";
        }
        out << std::endl << "Sides:" << std::endl;
        for (const Side& s : b.sides) {
            out << s << std::endl;
        }
        return out;
    }
};

//============================================================================//

int main() {
    int numberOfBuildings = 0;
    std::cin >> numberOfBuildings;
    Building b1;
    std::cin >> b1;
    b1.sort();
    std::vector<std::shared_ptr<Building>> buildings;
    for (int i = 2; i <= numberOfBuildings; ++i) {
        std::shared_ptr<Building> b2 = std::make_shared<Building>();
        std::cin >> *b2;
        b2->sort();
        buildings.push_back(b2);
    }
    //assert(rotations.size() == qrotations.size());
    std::vector<bool> matches(numberOfBuildings - 1, false);
    for (std::size_t j = 0; j < localRotations.size(); ++j) {
        if (std::all_of(matches.begin(), matches.end(),
                        [](bool b) { return b; })) {
            break;
        }
        //Building b = b1;
        b1.rotate(localRotations[j]);
        b1.sort();
        for (std::size_t k = 0; k < buildings.size(); ++k) {
            if (matches[k]) {
                continue; // already matched
            }
            matches[k] = b1 == *buildings[k];
        }
    }

    for (std::size_t i = 0; i < matches.size(); ++i) {
        if (matches[i]) {
            std::cout << (i+2) << ' ' << std::endl;
        }
    }
    std::cout << std::endl;
}

//============================================================================//
