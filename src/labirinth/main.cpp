#include "Field.hpp"
#include "HexMatrix.hpp"
#include "MatrixPropertyMap.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <array>
#include <iostream>
#include <map>

Matrix<Field> readInput(std::istream& stream) {
    return hex::readEcosim<Field>(stream);
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
class PrinterVisitor {
public:
    using event_filter = boost::on_tree_edge;

    PrinterVisitor(const DistanceMap& distanceMap) : distanceMap(distanceMap) {
    }

    template<typename Edge, typename Graph>
    void operator()(const Edge& edge, const Graph& graph) {
        auto from = source(edge, graph);
        auto to = target(edge, graph);
        std::cout << from << " --> " << to << " "
                << get(distanceMap, to) << "\n";
    }

private:
    const DistanceMap& distanceMap;
};

template<typename DistanceMap>
auto makePrinterVisitor(const DistanceMap& distanceMap) {
    return PrinterVisitor<DistanceMap>{distanceMap};
}

class Graph {
public:
    using Vertex = Point;
    using Edge = std::pair<Point, Point>;
    using Edges = std::vector<Edge>;

    Graph(const Matrix<Field>& inputMatrix) :
            matrix(inputMatrix.width() + 4, inputMatrix.height() + 4,
                    Field::corridor),
            edges{inputMatrix.width() + 4, inputMatrix.height() + 4} {
        for (Point p : matrixRange(inputMatrix)) {
            matrix[p + p11 * 2] = inputMatrix[p];
        }
        int width = this->matrix.width();
        int height =this->matrix.height();
        addEscapePoints(Point{0, 0}, p10);
        addEscapePoints(Point{0, height - 1}, p10);
        addEscapePoints(Point{0, 0}, p01);
        addEscapePoints(Point{width - 1, 0}, p01);
    }

    const Edges& getEdges(Point p) const {
        if (!isInsideMatrix(matrix, p)) {
            return getEscapeEdges(p);
        }
        if (!edges[p]) {
            edges[p] = calculateEdges(p);
        }
        return *edges[p];
    }

    const Matrix<Field>& getMatrix() const { return matrix; }
private:
    Edges calculateEdges(Point base) const {
        if (matrix[base] == Field::corridor && isStraightCorridor(base)) {
            return {};
        }

        Edges result;
        for (std::size_t direction = 0; direction < hex::numNeighbors;
                ++direction) {
            for (Point p = base + hex::getNeighbors(base)[direction];
                    isPassable(matrixAt(matrix, p, Field::wall));
                    p += hex::getNeighbors(p)[direction]) {
                result.emplace_back(base, p);
                if (matrix[p] == Field::monitor) {
                    break;
                }
            }
        }
        return result;
    }

    const Edges& getEscapeEdges(Point from) const {
        auto emplaceResult = escapeEdges.emplace(from, Edges{});
        Edges& result = emplaceResult.first->second;
        if (emplaceResult.second) {
            std::transform(escapePoints.begin(), escapePoints.end(),
                    std::back_inserter(result),
                    [from](Point to) { return std::make_pair(from, to); });
        }
        return result;
    }

    bool isStraightCorridor(Point p) const {
        const auto& neighbors = hex::getNeighbors(p);
        int passableNeighbors[2] = {-1, -1};
        std::size_t found = 0;
        for (std::size_t direction = 0; direction < hex::numNeighbors;
                ++direction) {
            Point pp = p + neighbors[direction];
            if (!isInsideMatrix(matrix, pp)) {
                // We are at the edge.
                return false;
            }
            if (isPassable(matrix[pp])) {
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

    void addEscapePoints(Point start, Point direction) const {
        for (Point p = start; isInsideMatrix(matrix, p); p += direction) {
            assert(matrix[p] == Field::corridor);
            escapePoints.push_back(p);
        }
    }

    Matrix<Field> matrix;
    mutable Matrix<boost::optional<Edges>> edges;
    mutable std::unordered_map<Point, Edges> escapeEdges;
    mutable std::vector<Point> escapePoints;
};

Point source(const Graph::Edge& edge, const Graph&) {
    return edge.first;
}

Point target(const Graph::Edge& edge, const Graph&) {
    return edge.second;
}

std::size_t out_degree(Graph::Vertex vertex, const Graph& graph) {
    return graph.getEdges(vertex).size();
}

std::pair<Graph::Edges::const_iterator, Graph::Edges::const_iterator>
out_edges(Graph::Vertex vertex, const Graph& graph) {
    const auto& edges = graph.getEdges(vertex);
    return std::make_pair(edges.cbegin(), edges.cend());
}

namespace boost {

template<>
struct graph_traits<Graph> {
    using vertex_descriptor = Graph::Vertex;
    using edge_descriptor = Graph::Edge;
    using directed_category = directed_tag;
    using edge_parallel_category = disallow_parallel_edge_tag;
    using traversal_category = incidence_graph_tag;
    using out_edge_iterator = Graph::Edges::const_iterator;
    using degree_size_type = std::size_t;
};

};

void printDistances(const Graph& graph, const Matrix<int> distances) {
    Matrix<std::string> result{graph.getMatrix().width(),
            graph.getMatrix().height()};
    for (Point p : matrixRange(graph.getMatrix())) {
        using std::to_string;
        result[p] = to_string(graph.getMatrix()[p]) + to_string(distances[p]);
    }
    hex::printMatrix(std::cerr, result);
    std::cerr << "\n";
}

Solution solve(const Graph& graph) {
    Matrix<int> distances{graph.getMatrix().width() + 1,
            graph.getMatrix().height(), 0};
    Matrix<boost::default_color_type> colors{graph.getMatrix().width() + 1,
            graph.getMatrix().height(), boost::white_color};
    boost::breadth_first_visit(graph,
            Point{static_cast<int>(graph.getMatrix().width()), 0},
            boost::visitor(boost::make_bfs_visitor(//std::make_pair(
                    boost::record_distances(matrixPropertyMap(distances),
                            boost::on_tree_edge())
                   // , makePrinterVisitor(matrixPropertyMap(distances)))
                    )).color_map(matrixPropertyMap(colors)));
    auto maxDistance = *std::max_element(distances.begin(),
            distances.end());
    Solution solution;
    solution.distance = maxDistance - 1;
    solution.halfWidth = graph.getMatrix().width() / 2 - 2;
    for (Point p : matrixRange(graph.getMatrix())) {
        if (distances[p] == maxDistance) {
            std::cerr << "-> " << p << "\n";
            solution.coordinates.push_back(p - p11 * 2);
        }
    }
    printDistances(graph, distances);
    return solution;
}

void printSolution(std::ostream& os, const Solution& solution) {
    os << solution.coordinates.size() << " " << solution.distance <<
            "\n";
    std::vector<Point> output;
    output.reserve(solution.coordinates.size());
    for (Point coordinate : solution.coordinates) {
        output.push_back(hex::local2ecosim(coordinate, solution.halfWidth));
    }
    std::sort(output.begin(), output.end(),
            [](Point lhs, Point rhs) {
                return lhs.y < rhs.y || (lhs.y == rhs.y && lhs.x < rhs.x);
            });
    for (Point ecosim : output) {
        os << ecosim.y << " " << ecosim.x << "\n";
    }
}

int main() {
    auto input = readInput(std::cin);
    hex::printMatrix(std::cerr, input);
    std::cerr << "\n";
    Solution solution = solve(std::move(input));
    printSolution(std::cout, solution);
}
