#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <unordered_map>
#include <vector>

//============================================================================//

// Number of rotation matrices: 24
using RotationMatrix = std::array<std::array<int, 3>, 3>;
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

bool operator==(const Vertex& lhs, const Vertex& rhs) {
    return lhs.refs == rhs.refs && lhs.x == rhs.x && lhs.y == rhs.y &&
            lhs.z == rhs.z;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//Vertex operator-(const Vertex& lhs, const Vertex& rhs) {
//    lhs.offset(
//    return lhs;
//    //return {rhs.x - lhs.x, rhs.y - lhs.y, rhs.z - lhs.z};
//}
//
//Vertex operator+(const Vertex& lhs, const Vertex& rhs) {
//    //return {rhs.x + lhs.x, rhs.y + lhs.y, rhs.z + lhs.z, lhs.refs};
//}

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
        for (Vertex*& v : vertices) {
            Vertex o = *v;
            v->x = r[0][0] * o.x + r[0][1] * o.y + r[0][2] * o.z;
            v->y = r[1][0] * o.x + r[1][1] * o.y + r[1][2] * o.z;
            v->z = r[2][0] * o.x + r[2][1] * o.y + r[2][2] * o.z;
        }
        //sort();
    }

    bool operator==(const Building& rhs) const {
        assert(isSorted);
        bool verticesEqual = vertices.size() == rhs.vertices.size() &&
                std::equal(vertices.begin(), vertices.end(),
                        rhs.vertices.begin(), rhs.vertices.end(),
                        equalsByValue<Vertex>);
        if (!verticesEqual) {
            return false;
        }
        bool verticeRefsEqual = vertices.size() == rhs.vertices.size() &&
                std::equal(vertices.begin(), vertices.end(),
                        rhs.vertices.begin(), rhs.vertices.end(),
                        equalsByRefs);
        if (!verticeRefsEqual) {
            return false;
        }
        bool edgesEqual = edges.size() == rhs.edges.size() &&
                std::equal(edges.begin(), edges.end(),
                        rhs.edges.begin(), rhs.edges.end(),
                        equalsByValue<Edge>);
        if (!edgesEqual) {
            return false;
        }
        bool sidesEqual = sides.size() == rhs.sides.size() &&
                std::equal(sides.begin(), sides.end(),
                        rhs.sides.begin(), rhs.sides.end());
        if (!sidesEqual) {
            return false;
        }
        return true;
        //return verticesEqual && verticeRefsEqual && edgesEqual && sidesEqual;
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

bool check(const Building& b1, Building b2, RotationMatrix r) {
    b2.rotate(r);
    //std::cerr << b2 << std::endl;
    Vertex offset = b1.calculateOffset(b2);
    std::cerr << offset << std::endl;
    b2.shift(offset);
    //std::cerr << b2 << std::endl;
    b2.sort();
    //std::cerr << b2 << std::endl;
    std::cerr << "b1: " << std::endl << b1 << std::endl << "b2: "
              << std::endl << b2 << std::endl;
    return b1 == b2;
}

int main() {
    int numberOfBuildings = 0;
    std::cin >> numberOfBuildings;
    Building b1;
    std::cin >> b1;
    b1.sort();
    for (int i = 2; i <= numberOfBuildings; ++i) {
        Building b2;
        std::cin >> b2;
        for (const auto& r : rotations) {
            if (check(b1, b2, r)) {
                std::cout << i << " ";
                break;
            }
        }
    }
    std::cout << std::endl;
}

//============================================================================//
