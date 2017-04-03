#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <vector>

//============================================================================//

// Number of rotation matrices: 24
using RotationMatrix = std::array<std::array<int, 3>, 3>;
const std::vector<RotationMatrix> rotations = {
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
    {{
        {{1, 0, 0}},
        {{0, 1, 0}},
        {{0, 0, 1}},
    }},
};

//============================================================================//

template<typename T>
bool lessThanByValue(T* lhs, T* rhs) {
    return *lhs < *rhs;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

template<typename T>
bool equalsByValue(T* lhs, T* rhs) {
    return *lhs == *rhs;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

struct Vertex {
    Vertex(int x, int y, int z) : x(x), y(y), z(z) {
    }

    int x = 0, y = 0, z = 0;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

bool operator<(const Vertex& lhs, const Vertex& rhs) {
    if (lhs.x == rhs.x) {
        if (lhs.y == rhs.y) {
            return lhs.z < rhs.z;
        }
        return lhs.y < rhs.y;
    }
    return lhs.x < rhs.x;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

bool operator==(const Vertex& lhs, const Vertex& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Vertex operator-(const Vertex& lhs, const Vertex& rhs) {
    return {rhs.x - lhs.x, rhs.y - lhs.y, rhs.z - lhs.z};
}

Vertex operator+(const Vertex& lhs, const Vertex& rhs) {
    return {rhs.x + lhs.x, rhs.y + lhs.y, rhs.z + lhs.z};
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

std::ostream& operator<<(std::ostream& out, const Vertex& v) {
    return out << "v(" << v.x << ',' << v.y << ',' << v.z << ')';
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

std::istream& operator>>(std::istream& in, Vertex& v) {
    return in >> v.x >> v.y >> v.z;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

struct Edge {
    Edge(Vertex* v1, Vertex* v2) : v1(v1), v2(v2) {
        if (*v2 < *v1) {
            std::swap(this->v1, this->v2);
        }
    }

    Vertex* v1, * v2;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

bool operator<(const Edge& lhs, const Edge& rhs) {
    if (*lhs.v1 == *rhs.v1) {
        return *lhs.v2 < *rhs.v2;
    }
    return *lhs.v1 < *rhs.v1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

bool operator==(const Edge& lhs, const Edge& rhs) {
    return *lhs.v1 == *rhs.v1 && *lhs.v2 == *rhs.v2;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

std::ostream& operator<<(std::ostream& out, const Edge& e) {
    return out << "e(" << *e.v1 << ',' << * e.v2 << ')';
}

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

bool operator==(const Side& lhs, const Side& rhs) {
    bool equalEdges = lhs.edges.size() == rhs.edges.size() &&
            std::equal(lhs.edges.begin(), lhs.edges.end(),
                    rhs.edges.begin(), rhs.edges.end(), equalsByValue<Edge>);
    bool equalHoles = lhs.holes.size() == rhs.holes.size() &&
            std::equal(lhs.holes.begin(), lhs.holes.end(),
                    rhs.holes.begin(), rhs.holes.end());
    return equalEdges && equalHoles;
}

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

    void sort() {
        std::sort(vertices.begin(), vertices.end(), lessThanByValue<Vertex>);
        std::sort(edges.begin(), edges.end(), lessThanByValue<Edge>);
        for (auto& side : sides) {
            side.sort();
        }
        std::sort(sides.begin(), sides.end());
        isSorted = true;
    }

    void copy(const Building& other) {
        if (this == &other) {
            return;
        }
        assert(vertices.empty());
        assert(edges.empty());
        assert(sides.empty());
        std::map<const Vertex*, Vertex*> vTranslationTable;
        for (const Vertex* const& v : other.vertices) {
            Vertex* vertex = new Vertex(v->x, v->y, v->z);
            vertices.push_back(vertex);
            vTranslationTable[v] = vertex;
        }
        std::map<const Edge*, Edge*> eTranslationTable;
        for (const Edge* const& e : other.edges) {
            Edge* edge = new Edge(vTranslationTable[e->v1],
                    vTranslationTable[e->v2]);
            edges.push_back(edge);
            eTranslationTable[e] = edge;
        }
        for (const Side& s : other.sides) {
            Side side;
            for (const Edge* e : s.edges) {
                side.edges.push_back(eTranslationTable[e]);
            }
            for (const Side& h : s.holes) {
                Side hole;
                for (const Edge* he : h.edges) {
                    hole.edges.push_back(eTranslationTable[he]);
                }
                side.holes.push_back(hole);
            }
            sides.push_back(side);
        }
    }

public:
    Building() = default;

    Building(const Building& other) {
        copy(other);
    }

    Building(Building&&) = delete;

    Building& operator=(const Building& other) {
        copy(other);
        return *this;
    }

    Building& operator=(Building&&) = delete;

    ~Building() {
        for (Vertex*& v : vertices) {
            delete v;
        }
        for (Edge*& e : edges) {
            delete e;
        }
    }

    Vertex calculateOffset(const Building& rhs) const {
        return *rhs.vertices[0] - *vertices[0];
    }


    void shift(const Vertex& offset) {
        //std::cout << "shifting with: " << offset << std::endl;
        for (Vertex*& v : vertices) {
            *v = *v + offset;
        }
    }

    void rotate(const RotationMatrix& r) {
        for (Vertex*& v : vertices) {
            Vertex o = *v;
            v->x = r[0][0] * o.x + r[0][1] * o.y + r[0][2] * o.z;
            v->y = r[1][0] * o.x + r[1][1] * o.y + r[1][2] * o.z;
            v->z = r[2][0] * o.x + r[2][1] * o.y + r[2][2] * o.z;
        }
        sort();
    }

    bool operator==(const Building& rhs) const {
        assert(isSorted);
        bool verticesEqual = vertices.size() == rhs.vertices.size() &&
                std::equal(vertices.begin(), vertices.end(),
                        rhs.vertices.begin(), rhs.vertices.end(),
                        equalsByValue<Vertex>);
        bool edgesEqual = edges.size() == rhs.edges.size() &&
                std::equal(edges.begin(), edges.end(),
                        rhs.edges.begin(), rhs.edges.end(),
                        equalsByValue<Edge>);
        bool sidesEqual = sides.size() == rhs.sides.size() &&
                std::equal(sides.begin(), sides.end(),
                        rhs.sides.begin(), rhs.sides.end());
        return verticesEqual && edgesEqual && sidesEqual;
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
                side.edges.push_back(b.edges[edge - 1]); // 1-based
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
                    hole.edges.push_back(b.edges[edge - 1]); // 1-based
                }
                side.holes.push_back(hole);
            }
            b.addSide(side);
        }
        b.sort();
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

bool check(const Building& b1, Building b2, RotationMatrix r) {
    b2.rotate(r);
    Vertex offset = b1.calculateOffset(b2);
    //std::cout << offset << std::endl;
    b2.shift(offset);
    //std::cout << b1 << std::endl << b2 << std::endl;
    return b1 == b2;
}

int main() {
    Building b1, b2;
    std::cin >> b1 >> b2;
    for (const auto& r : rotations) {
        if (check(b1, b2, r)) {
            std::cout << "TRUE" << std::endl;
            return 0;
        }
    }
    std::cout << "FALSE" << std::endl;
}

//============================================================================//
