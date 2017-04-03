#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dag_shortest_paths.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/property_map/function_property_map.hpp>
#include <boost/functional/hash.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <algorithm>
#include <numeric>
#include <iostream>
#include <random>
#include <vector>

struct Ferry {
    std::size_t from;
    std::size_t to;
    int time;
    int skippedBikeTime;
};

struct Problem {
    int timeLimit;
    std::vector<std::string> cityNames;
    std::vector<int> bikePaths;
    int totalBikePathLength;

    std::vector<Ferry> ferries;
};

class VertexIterator : public boost::iterator_facade<
        VertexIterator, std::size_t, boost::random_access_traversal_tag,
        std::size_t> {
public:
    VertexIterator() : value(0) {}

    VertexIterator(std::size_t value) : value(value) {}

    std::size_t dereference() const { return value; }

    bool equal(const VertexIterator& other) const {
        return other.value == value;
    }

    void increment() { ++value; }
    void decrement() { --value; }
    void advance(std::ptrdiff_t n) {
        value += n;
    }

    std::ptrdiff_t distance_to(const VertexIterator& other) {
        return other.value - value;
    }

private:
    std::size_t value;
};

using Edge = std::pair<std::size_t, std::size_t>;

class EdgeIterator : public boost::iterator_facade<
        EdgeIterator, Edge, boost::random_access_traversal_tag, Edge> {
private:
    using InnerIterator = std::vector<Ferry>::const_iterator;
public:
    EdgeIterator() {
        // std::cerr << this << ": default construct\n";
    }

    EdgeIterator(InnerIterator begin, InnerIterator iterator,
            std::size_t vertex, std::size_t numVertices) :
            begin(begin), iterator(iterator), vertex(vertex),
            numVertices(numVertices) {
        // std::cerr << this << ": construct: " << std::distance(begin, iterator)
        //         << "\n";
    }

    Edge dereference() const {
        // std::cerr << this << ": dereference: " << std::distance(begin, iterator)
        //         << "\n";
        if (isAtBeginning()) {
            return {vertex, (vertex + 1) % numVertices};
        }
        return {iterator->from, iterator->to};
    }

    bool equal(const EdgeIterator& other) const {
        return other.iterator == iterator;
    }

    void increment() {
        ++iterator;
        // std::cerr << this << ": increment: " << std::distance(begin, iterator)
        //         << "\n";
    }
    void decrement() {
        --iterator;
        // std::cerr << this << ": decrement: " << std::distance(begin, iterator)
        //         << "\n";
    }
    void advance(std::ptrdiff_t n) {
        iterator += n;
        // std::cerr << this << ": advance " << n << ": "
        //         << std::distance(begin, iterator) << "\n";
    }

    std::ptrdiff_t distance_to(const EdgeIterator& other) {
        return std::distance(iterator, other.iterator);
    }

private:
    InnerIterator begin;
    InnerIterator iterator;
    std::size_t vertex;
    std::size_t numVertices;

    bool isAtBeginning() const {
        return std::distance(begin, iterator) < 0;
    }
};

std::size_t num_vertices(const Problem& graph) {
    return graph.bikePaths.size() + 1;
}

auto vertices(const Problem& graph) {
    return std::make_pair(VertexIterator{0},
            VertexIterator{num_vertices(graph)});
}

std::size_t source(const Edge& edge, const Problem&) {
    return edge.first;
}

std::size_t target(const Edge& edge, const Problem& graph) {
    return edge.second != 0 ? edge.second : num_vertices(graph) - 1;
}

struct EdgeComparator {
    bool operator()(const Ferry& ferry, std::size_t vertex) {
        return ferry.from < vertex;
    }

    bool operator()(std::size_t vertex, const Ferry& ferry) {
        return vertex < ferry.from;
    }
};

auto out_edges(std::size_t vertex, const Problem& problem) {
    auto range = std::equal_range(problem.ferries.begin(),
            problem.ferries.end(), vertex, EdgeComparator{});
    int offset = (vertex == num_vertices(problem) - 1) ? 0 : -1;
    return std::make_pair(
            EdgeIterator{range.first, range.first + offset, vertex,
                    num_vertices(problem)},
            EdgeIterator{range.first, range.second, vertex,
                    num_vertices(problem)});
}

std::size_t out_degree(std::size_t vertex, const Problem& problem) {
    auto range = std::equal_range(problem.ferries.begin(),
            problem.ferries.end(), vertex, EdgeComparator{});
    return std::distance(range.first, range.second) + 1;
}

boost::identity_property_map get(boost::vertex_index_t, const Problem&) {
    return {};
}

bool isBikePath(const Edge& edge, const Problem& problem) {
    return target(edge, problem) - source(edge, problem) == 1;
}

const Ferry& getFerry(const Edge& edge, const Problem& problem) {
    auto range = std::equal_range(problem.ferries.begin(),
            problem.ferries.end(), source(edge, problem), EdgeComparator{});
    return *std::find_if(range.first, range.second,
            [&edge](const Ferry& ferry) {
                return ferry.to == edge.second;
            });
}

auto get(boost::edge_weight_t, const Problem& problem) {
    return boost::make_function_property_map<Edge>(
            [&problem](const Edge& edge) {
                if (isBikePath(edge, problem)) {
                    return problem.bikePaths[edge.first];
                }
                return getFerry(edge, problem).time;
            });
}

struct DebugVisitor {
    template<typename U, typename G>
    void initialize_vertex(U u, const G& g) {
        printVertex("Initialize", u, g);
    }

    template<typename U, typename G>
    void examine_vertex(U u, const G& g) {
        printVertex("Examine", u, g);
    }

    template<typename E, typename G>
    void examine_edge(E e, const G& g) {
        printEdge("Examine", e, g);
    }

    template<typename U, typename G>
    void discover_vertex(U u, const G& g) {
        printVertex("Discover", u, g);
    }

    template<typename E, typename G>
    void edge_relaxed(E e, const G& g) {
        printEdge("Relaxed", e, g);
    }

    template<typename E, typename G>
    void edge_not_relaxed(E e, const G& g) {
        printEdge("Not relaxed", e, g);
    }

    template<typename U, typename G>
    void finish_vertex(U u, const G& g) {
        printVertex("Finish", u, g);
    }

    template<typename U, typename G>
    void printVertex(const char* label, U u, const G&) {
        std::cerr << label << ": " << u << "\n";
    }

    template<typename E, typename G>
    void printEdge(const char* label, E e, const G& g) {
        std::cerr << label << ": " << source(e, g) << "->" << target(e, g)
                << " w=" << get(get(boost::edge_weight, g), e) << "\n";
    }
};

namespace boost {

template<>
struct graph_traits<Problem> {
    using vertex_descriptor = std::size_t;
    using edge_descriptor = Edge;
    using directed_category = directed_tag;
    using edge_parallel_category = disallow_parallel_edge_tag;
    struct traversal_category : incidence_graph_tag, vertex_list_graph_tag {};
    using out_edge_iterator = EdgeIterator;
    using degree_size_type = std::size_t;
    using vertex_iterator = VertexIterator;
    using vertices_size_type = std::size_t;
};

template<>
struct property_map<Problem, vertex_index_t> {
    using const_type = identity_property_map;
};

template<>
struct property_map<Problem, edge_weight_t> {
    using const_type = decltype(get(edge_weight, std::declval<Problem>()));
};

}

Problem readInput(std::istream& stream) {
    Problem problem;
    std::size_t numCities = 0;
    stream >> numCities;
    problem.cityNames.reserve(numCities);
    std::map<std::string, std::size_t> cityNames;
    for (std::size_t i = 0; i < numCities; ++i) {
        std::string name;
        stream >> name;
        problem.cityNames.push_back(name);
        cityNames.emplace(name, i);
    }
    problem.bikePaths.reserve(numCities);
    problem.totalBikePathLength = 0;
    for (std::size_t i = 0; i < numCities; ++i) {
        int time = 0;
        stream >> time;
        problem.bikePaths.push_back(time);
        problem.totalBikePathLength += time;
    }

    std::size_t numFerries = 0;
    stream >> numFerries;
    for (std::size_t i = 0; i < numFerries; ++i) {
        std::string from, to;
        int time = 0;
        stream >> from >> to >> time;
        std::size_t fromIndex = cityNames.at(from);
        std::size_t toIndex = cityNames.at(to);
        problem.ferries.push_back(Ferry{fromIndex, toIndex, time,
                std::accumulate(
                        problem.bikePaths.begin() + fromIndex,
                        problem.bikePaths.begin() + toIndex, 0)});
    }
    std::sort(problem.ferries.begin(), problem.ferries.end(),
            [](const Ferry& lhs, const Ferry& rhs) {
                return lhs.from < rhs.from;
            });

    stream >> problem.timeLimit;
    return problem;
}

template<typename Container, typename Function, typename RNG>
typename Container::iterator getRandomElement(Container& elements, RNG& rng,
        const Function& weightFunction) {
    auto range = elements | boost::adaptors::transformed(weightFunction);
    std::discrete_distribution<std::size_t> distribution{
            range.begin(), range.end()};
    return elements.begin() + distribution(rng);
}

class Solver {
public:
    Solver(Problem problem) :
            problem(std::move(problem)), bikeTime(0), totalTime(0) {
    }

    void solve() {
        // std::cerr << "Ferries:\n";
        // for (const Ferry& ferry : problem.ferries) {
        //     std::cerr << ferry.from << "->" << ferry.to << " t=" << ferry.time
        //             << " bt=" << ferry.skippedBikeTime << "\n";
        // }
        // std::cerr << "Graph:\n";
        // for (auto vertex : boost::make_iterator_range(vertices(problem))) {
        //     std::cerr << "Vertex: " << vertex << "\n";
        //     for (auto edge : boost::make_iterator_range(
        //             out_edges(vertex, problem))) {
        //         std::cerr << "  Edge: " << edge.first << "->"
        //                 << target(edge, problem) << " "
        //                 << get(get(boost::edge_weight, problem), edge) << "\n";
        //     }
        // }
        findShortestPath();
        bestBikeTime = 0;
        // TODO: Use time limit.
        for (int i = 0; i < 100; ++i) {
            while (totalTime <= problem.timeLimit) {
                removeFerry();
            }
            while (totalTime > problem.timeLimit) {
                addFerry();
            }
        }
    }

    auto getResult() const {
        std::vector<std::pair<const std::string&, const std::string&>> result;
        result.reserve(bestSolution.size());
        for (const auto& ferry : bestSolution) {
            result.emplace_back(problem.cityNames[ferry->from],
                    problem.cityNames[ferry->to]);
        }
        return result;
    }

private:
    using FerrySet = boost::container::flat_set<const Ferry*>;

    void findShortestPath() {
        std::vector<std::size_t> predecessors(num_vertices(problem));
        std::vector<int> distances(num_vertices(problem));
        boost::dag_shortest_paths(problem, 0,
                boost::predecessor_map(&predecessors[0])
                .distance_map(&distances[0])
                /* .visitor(DebugVisitor{}) */);
        totalTime = distances.back();
        bikeTime = 0;
        for (std::size_t vertex = num_vertices(problem) - 1;
                vertex != 0; vertex = predecessors[vertex]) {
            auto edge = std::make_pair(predecessors[vertex], vertex);
            const Ferry* exclude = nullptr;
            if (isBikePath(edge, problem)) {
                int weight = get(get(boost::edge_weight, problem), edge);
                bikeTime += weight;
            } else {
                const Ferry& ferry = getFerry(edge, problem);
                std::cerr << "Using ferry: " << problem.cityNames[ferry.from]
                        << " -> " << problem.cityNames[ferry.to]
                        << " t=" << ferry.time
                        << " bt=" << ferry.skippedBikeTime << "\n";
                usedFerries.insert(&ferry);
                exclude = &ferry;
            }

            auto begin = std::lower_bound(problem.ferries.begin(),
                    problem.ferries.end(), predecessors[vertex],
                    EdgeComparator{});
            auto end = std::lower_bound(problem.ferries.begin(),
                    problem.ferries.end(), vertex, EdgeComparator{});
            for (auto iterator = begin; iterator != end; ++iterator) {
                const Ferry* ferry = &*iterator;
                if (ferry != exclude) {
                    usableFerries.insert(ferry);
                }
            }
        }
    }

    void addUsableFerries(std::size_t begin, std::size_t end) {
        for (const Ferry& ferry : problem.ferries) {
            if ((ferry.from > begin && ferry.from < end)
                    || (ferry.to > begin && ferry.to < end)) {
                usableFerries.insert(&ferry);
            }
        }
    }

    void removeUsableFerries(std::size_t begin, std::size_t end) {
        std::vector<const Ferry*> currentFerries(usableFerries.size());
        std::copy(usableFerries.begin(), usableFerries.end(),
                currentFerries.begin());
        for (const Ferry* ferry : currentFerries) {
            if ((ferry->from > begin && ferry->from < end)
                    || (ferry->to > begin && ferry->to < end)) {
                usableFerries.erase(ferry);
            }
        }
    }

    void removeFerry() {
        auto iterator = getRandomElement(usedFerries, rng,
                [](const Ferry* ferry) {
                    return ferry->skippedBikeTime - ferry->time;
                });
        const Ferry* ferry = *iterator;

        int newTotalTime = totalTime + ferry->skippedBikeTime - ferry->time;
        if (newTotalTime > problem.timeLimit) {
            if (bikeTime > bestBikeTime) {
                std::cerr << "New best solution: bike time = " << bikeTime
                        << " total time = " << totalTime << "\n";
                bestSolution.resize(usedFerries.size());
                std::copy(usedFerries.begin(), usedFerries.end(),
                        bestSolution.begin());
                bestBikeTime = bikeTime;
            }
        }

        // std::cerr << "Removing ferry from " << problem.cityNames[ferry->from]
        //         << " to " << problem.cityNames[ferry->to] << "\n";
        usedFerries.erase(iterator);
        bikeTime += ferry->skippedBikeTime;
        totalTime = newTotalTime;
        addUsableFerries(ferry->from, ferry->to);
    }

    void addFerry() {
        auto iterator = getRandomElement(usableFerries, rng,
                [](const Ferry* ferry) {
                    return 1.0 / (ferry->skippedBikeTime - ferry->time);
                });
        const Ferry* ferry = *iterator;
        // std::cerr << "Adding ferry from " << problem.cityNames[ferry->from]
        //         << " to " << problem.cityNames[ferry->to] << "\n";
        usedFerries.insert(ferry);
        bikeTime -= ferry->skippedBikeTime;
        totalTime -= ferry->skippedBikeTime - ferry->time;
        removeUsableFerries(ferry->from, ferry->to);
    }

    Problem problem;
    FerrySet usedFerries;
    FerrySet usableFerries;
    std::vector<const Ferry*> bestSolution;
    int bestBikeTime;
    int bikeTime;
    int totalTime;
    std::mt19937 rng{std::random_device{}()};
};

int main() {
    Solver solver{readInput(std::cin)};
    solver.solve();
    auto solution = solver.getResult();
    std::cout << solution.size() << "\n";
    for (const auto& ferry : solution) {
        std::cout << ferry.first << " " << ferry.second << "\n";
    }
}