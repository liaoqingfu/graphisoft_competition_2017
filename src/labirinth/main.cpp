#include "Field.hpp"
#include "HexMatrix.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <array>
#include <iostream>
#include <map>

struct Input {
    Matrix<Field> matrix;
};

Input readInput(std::istream& stream) {
    return Input{hex::readEcosim<Field>(stream)};
}

struct Node {
    Point coordinate;

    Node(Point coordinate = Point{-1, -1}) : coordinate(coordinate) {}
};

struct Solution {
    int distance;
    int halfWidth;
    std::vector<Point> coordinates;
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

    Solution solve() {
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
        Solution solution;
        solution.distance = maxDistance - 1;
        solution.halfWidth = input.matrix.width() / 2;
        for (std::size_t i = 0; i < distances.size(); ++i) {
            if (distances[i] == maxDistance) {
                solution.coordinates.push_back(
                        graph[vertex(i, graph)].coordinate);
            }
        }
        return solution;
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
        for (std::size_t direction = 0; direction < hex::numNeighbors;
                ++direction) {
            for (Point p = base + hex::getNeighbors(base)[direction];
                    isPassable(matrixAt(input.matrix, p, Field::wall));
                    p += hex::getNeighbors(p)[direction]) {
                boost::add_edge(endpoint, getVertex(p), graph);
                if (input.matrix[p] == Field::monitor) {
                    break;
                }
            }
        }
    }

    bool isStraightCorridor(Point p) const {
        const auto& neighbors = hex::getNeighbors(p);
        int passableNeighbors[2] = {-1, -1};
        std::size_t found = 0;
        for (std::size_t direction = 0; direction < hex::numNeighbors;
                ++direction) {
            Point pp = p + neighbors[direction];
            if (!isInsideMatrix(input.matrix, pp)) {
                // We are at the edge.
                return false;
            }
            if (isPassable(input.matrix[pp])) {
                if (found == 2) {
                    // There are more than 2 neighboring corridors.
                    return false;
                }
                passableNeighbors[found++] = direction;
            }
        }
        return found == 2 && passableNeighbors[1] - passableNeighbors[0] ==
                hex::numNeighbors / 2;
    }

    void addEscapePoints(Point start, Point direction) {
        for (Point p = start; isInsideMatrix(input.matrix, p); p += direction) {
            if (isPassable(input.matrix[p])) {
                boost::add_edge(escapeVertex, getVertex(p), graph);
            }
        }
    }

    Input input;
    Graph graph;
    std::unordered_map<Point, Vertex> vertices;
    Vertex escapeVertex;
};

void printSolution(std::ostream& os, const Solution& solution) {
    os << solution.coordinates.size() << " " << solution.distance <<
            "\n";
    for (Point coordinate : solution.coordinates) {
        Point ecosim = hex::local2ecosim(coordinate, solution.halfWidth);
        os << ecosim.y << " " << ecosim.x << "\n";
    }
}

int main() {
    auto input = readInput(std::cin);
    hex::printMatrix(std::cerr, input.matrix);
    Solver solver{std::move(input)};
    solver.createGraph();
    // solver.printGraph();
    Solution solution = solver.solve();
    printSolution(std::cout, solution);
}
