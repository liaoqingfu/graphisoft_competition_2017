#include "Matrix.hpp"
#include "MatrixIO.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/property_map/property_map.hpp>
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

std::string to_string(Field node) {
    return std::string{static_cast<char>(node)};
}

struct Input {
    Matrix<Field> matrix;
};

void printInput(std::ostream& os, const Input& input) {
    os << "  ";
    for (int x = 0; x < static_cast<int>(input.matrix.width()); ++x) {
        os << x % 10 << " ";
    }
    os << "\n";
    for (int y = 0; y < static_cast<int>(2*input.matrix.height()); ++y) {
        if (y % 2 == 0) {
            os << (y / 2) % 10 << " ";
        } else {
            os << "  ";
        }
        for (int x = 0; x < static_cast<int>(input.matrix.width()); ++x) {
            if (x % 2 == y % 2) {
                os << to_string(input.matrix[Point{x, y / 2}]) << " ";
            } else {
                os << "  ";
            }
        }
        os << "\n";
    }
}

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


constexpr std::size_t numNeighbors = 6;
using Neighbors = std::array<Point, numNeighbors>;

constexpr Neighbors oddNeighbors{
        {-px, -p10, -p01, p10, p11, p01}};

constexpr Neighbors evenNeighbors{
        {-p10, -p11, -p01, px, p10, p01}};

constexpr const Neighbors& getNeighbors(Point p) {
    return p.x % 2 == 0 ? evenNeighbors : oddNeighbors;
}

struct Node {
    Point coordinate;

    Node(Point coordinate = Point{-1, -1}) : coordinate(coordinate) {}
};

template<typename DistanceMap>
struct PrinterVisitor {
    using event_filter = boost::on_tree_edge;

    PrinterVisitor(const DistanceMap& distanceMap) : distanceMap(distanceMap) {
    }

    template<typename Edge, typename Graph>
    void operator()(const Edge& edge, const Graph& graph) {
        auto from = source(edge, graph);
        auto to = target(edge, graph);
        std::cerr << get(distanceMap, from) << " " << graph[from].coordinate
                << " --> " << get(distanceMap, to) << " "
                << graph[to].coordinate << "\n";

    }

private:
    const DistanceMap& distanceMap;
};

template<typename DistanceMap>
auto makePrinterVisitor(const DistanceMap& distanceMap) {
    return PrinterVisitor<DistanceMap>{distanceMap};
}

class Solver {
public:
    using Graph = boost::adjacency_list<boost::vecS, boost::vecS,
            boost::directedS, Node>;
    using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
    using Edge = boost::graph_traits<Graph>::edge_descriptor;

    Solver(Input input) : input(std::move(input)) {}

    void createGraph() {
        for (Point p : matrixRange(input.matrix)) {
            if (input.matrix[p] == Field::monitor ||
                    (input.matrix[p] == Field::corridor
                            && !isStraightCorridor(p))) {
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

    void solve() {
        std::size_t size = boost::num_vertices(graph);
        std::vector<boost::graph_traits<Graph>::vertices_size_type> distances(
                size);
        auto distancesMap = boost::make_iterator_property_map(
                distances.begin(),
                boost::get(boost::vertex_index_t{}, graph));
        boost::breadth_first_search(graph, escapeVertex,
                boost::visitor(boost::make_bfs_visitor(std::make_pair(
                        boost::record_distances(distancesMap,
                                boost::on_tree_edge()),
                        makePrinterVisitor(distancesMap)))));
        auto maxDistance = *std::max_element(distances.begin(),
                distances.end());
        for (std::size_t i = 0; i < distances.size(); ++i) {
            if (distances[i] == maxDistance) {
                std::cout << graph[vertex(i, graph)].coordinate << "\n";
            }
        }
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
        // std::cerr << "addEdges " << base << "\n";
        Vertex endpoint = getVertex(base);
        for (std::size_t direction = 0; direction < numNeighbors; ++direction) {
            for (Point p = base + getNeighbors(base)[direction];
                    matrixAt(input.matrix, p, Field::wall) != Field::wall;
                    p += getNeighbors(p)[direction]) {
                boost::add_edge(endpoint, getVertex(p), graph);
                if (input.matrix[p] == Field::monitor) {
                    break;
                }
            }
        }
    }

    bool isStraightCorridor(Point p) const {
        const auto& neighbors = getNeighbors(p);
        int passableNeighbors[2] = {-1, -1};
        std::size_t found = 0;
        for (std::size_t direction = 0; direction < numNeighbors; ++direction) {
            Point pp = p + neighbors[direction];
            if (!isInsideMatrix(input.matrix, pp)) {
                // We are at the edge.
                return false;
            }
            if (input.matrix[pp] != Field::wall) {
                if (found == 2) {
                    // There are more than 2 neighboring corridors.
                    return false;
                }
                passableNeighbors[found++] = direction;
            }
        }
        return found == 2 && passableNeighbors[1] - passableNeighbors[0] ==
                numNeighbors / 2;
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
    auto input = readInput(std::cin);
    printInput(std::cerr, input);
    Solver solver{std::move(input)};
    solver.createGraph();
    // solver.printGraph();
    solver.solve();
}
