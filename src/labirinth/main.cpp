#include "Matrix.hpp"
#include "MatrixIO.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <array>
#include <iostream>
#include <map>

enum class Field : char {
    wall = 'W', corridor = 'C', monitor = 'M'
};

std::istream& operator>>(std::istream& is, Field& f) {
    char c;
    is >> c;
    f = static_cast<Field>(c);
    return is;
}

// std::string to_string(InputNode node) {
//     return std::string{static_cast<char>(node)};
// }
//
struct Input {
    Matrix<Field> matrix;
};

void readElement(std::istream& stream, Input& input, Point p) {
    Field& field = input.matrix[p];
    stream >> field;
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

using Neighbors = std::array<Point, 6>;

constexpr Neighbors oddNeighbors{
        {-p10, -p01, p10, Point{-1, 1}, p01, p11}};

constexpr Neighbors evenNeighbors{
        {-p11, -p01, Point{1, -1}, -p10, p01, p10}};

constexpr Neighbors getDirections(Point p) {
    return p.x % 2 == 0 ? evenNeighbors : oddNeighbors;
}

Neighbors getNeighbors(Point p) {
    Neighbors result;
    for (std::size_t i = 0; i < 6; ++i) {
        result[i] = p + getDirections(p)[i];
    }
    return result;
}

struct Node {
    Point coordinate;

    Node(Point coordinate = Point{-1, -1}) : coordinate(coordinate) {}
};

class Solver {
public:
    using Graph = boost::adjacency_list<boost::vecS, boost::vecS,
            boost::directedS, Node>;
    using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
    using Edge = boost::graph_traits<Graph>::edge_descriptor;

    Solver(Input input) : input(std::move(input)) {}

    void createGraph() {
        for (Point p : matrixRange(input.matrix)) {
            if (input.matrix[p] != Field::wall) {
                addEdges(p);
            }
        }

        escapeVertex = boost::add_vertex(graph);
        int width = input.matrix.width();
        int height = input.matrix.height();
        addEscapePoints(Point{0, 0}, p10);
        addEscapePoints(Point{0, height - 1}, p10);
        addEscapePoints(Point{0, 0}, p01);
        addEscapePoints(Point{width - 1, 0}, p01);
    }

    void printGraph() {
        for (const Edge& edge :
                boost::make_iterator_range(boost::edges(graph))) {
            std::cerr << graph[source(edge, graph)].coordinate
                    << " --> " << graph[target(edge, graph)].coordinate << "\n";
        }
    }

private:
    Vertex getVertex(Point p) {
        auto insertResult = vertices.emplace(p,
                boost::graph_traits<Graph>::null_vertex());
        if (insertResult.second) {
            insertResult.first->second = boost::add_vertex(Node{p}, graph);
        }
        return insertResult.first->second;
    }

    void addEdges(Point base) {
        Vertex endpoint = getVertex(base);
        for (const Point direction : getDirections(base)) {
            for (Point p = base + direction;
                    matrixAt(input.matrix, p, Field::wall) != Field::wall;
                    p += direction) {
                if (input.matrix[p] == Field::monitor) {
                    boost::add_edge(getVertex(p), endpoint, graph);
                    break;
                }
                if (!isStraightCorridor(p, direction)) {
                    boost::add_edge(getVertex(p), endpoint, graph);
                }
            }
        }
    }

    bool isStraightCorridor(Point p, Point direction) const {
        Point pd = p + direction;
        Point pmd = p - direction;
        for (Point p2 : getNeighbors(p)) {
            if (!isInsideMatrix(input.matrix, p2)) {
                return false;
            }
            if (p2 != pd && p2 != pmd &&
                    input.matrix[p2] != Field::wall) {
                return false;
            }
        }
        return input.matrix[pd] != Field::wall;
    }

    void addEscapePoints(Point start, Point direction) {
        for (Point p = start; isInsideMatrix(input.matrix, p); p += direction) {
            if (input.matrix[p] != Field::wall) {
                boost::add_edge(escapeVertex, getVertex(p), graph);
            }
        }
    }

    Input input;
    Graph graph;
    std::unordered_map<Point, Vertex> vertices;
    Vertex escapeVertex;
};

int main() {
    Solver solver{readInput(std::cin)};
    solver.createGraph();
    solver.printGraph();
}
