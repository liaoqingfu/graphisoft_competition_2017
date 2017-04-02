#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/property_map/function_property_map.hpp>

#include <algorithm>
#include <numeric>
#include <iostream>
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
    EdgeIterator() = default;

    EdgeIterator(InnerIterator begin, InnerIterator iterator,
            std::size_t vertex) :
            begin(begin), iterator(iterator), vertex(vertex) {}

    Edge dereference() const {
        if (isAtBeginning()) {
            return {vertex, vertex + 1};
        }
        return {iterator->from, iterator->to};
    }

    bool equal(const EdgeIterator& other) const {
        return other.iterator == iterator;
    }

    void increment() { ++iterator; }
    void decrement() { --iterator; }
    void advance(std::ptrdiff_t n) {
        iterator += n;
    }

    std::ptrdiff_t distance_to(const EdgeIterator& other) {
        return std::distance(iterator, other.iterator);
    }

private:
    InnerIterator begin;
    InnerIterator iterator;
    std::size_t vertex;

    bool isAtBeginning() const {
        return iterator < begin;
    }
};

auto vertices(const Problem& graph) {
    return std::make_pair(VertexIterator{0},
            VertexIterator{graph.bikePaths.size()});
}

std::size_t num_vertices(const Problem& graph) {
    return graph.bikePaths.size();
}

std::size_t source(const Edge& edge, const Problem&) {
    return edge.first;
}

std::size_t target(const Edge& edge, const Problem&) {
    return edge.second;
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
    return std::make_pair(
            EdgeIterator{range.first, range.first - 1, vertex},
            EdgeIterator{range.first, range.second, vertex});
}

std::size_t out_degree(std::size_t vertex, const Problem& problem) {
    auto range = std::equal_range(problem.ferries.begin(),
            problem.ferries.end(), vertex, EdgeComparator{});
    return std::distance(range.first, range.second) + 1;
}

boost::identity_property_map get(boost::vertex_index_t, const Problem&) {
    return {};
}

int positiveModulo(int a, int b)  {
    return ((a % b) + b) % b;
}

bool isBikePath(const Edge& edge, const Problem& problem) {
    return positiveModulo(edge.second - edge.first,
        problem.bikePaths.size()) == 1;
}

const Ferry& getFerry(const Edge& edge, const Problem& problem) {
    auto range = std::equal_range(problem.ferries.begin(),
            problem.ferries.end(), edge.first, EdgeComparator{});
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

class Solver {
public:
    Solver(Problem problem) :
            problem(std::move(problem)), bikeTime(0), totalTime(0) {
    }

    void solve() {
        findShortestPath();
        while (removeFerry()) {}
    }

    auto getResult() const {
        std::vector<const Ferry*> ferries(
                usedFerries.begin(), usedFerries.end());
        std::sort(ferries.begin(), ferries.end(),
                [](const Ferry* lhs, const Ferry* rhs) {
                    return lhs->from < rhs->from;
                });
        std::vector<std::pair<const std::string&, const std::string&>> result;
        result.reserve(ferries.size());
        for (const auto& ferry : ferries) {
            result.emplace_back(problem.cityNames[ferry->from],
                    problem.cityNames[ferry->to]);
        }
        return result;
    }

private:
    void findShortestPath() {
        std::vector<std::size_t> predecessors(problem.bikePaths.size());
        std::vector<int> distances(problem.bikePaths.size());
        boost::dijkstra_shortest_paths(problem, 0,
                boost::predecessor_map(&predecessors[0]).
                distance_map(&distances[0]));
        totalTime = distances.back() + problem.bikePaths.back();
        bikeTime = distances.back();
        for (std::size_t vertex = problem.bikePaths.size() - 1;
                vertex != 0; vertex = predecessors[vertex]) {
            auto edge = std::make_pair(predecessors[vertex], vertex);
            if (isBikePath(edge, problem)) {
                bikeTime += problem.bikePaths[predecessors[vertex]];
            } else {
                usedFerries.insert(&getFerry(edge, problem));
            }
        }
    }

    bool removeFerry() {
        std::cerr << "Total time: " << totalTime << " Bike time: "
                << bikeTime << "\n";
        int timeRemaining = problem.timeLimit - totalTime;
        assert(timeRemaining >= 0);
        FerrySet::const_iterator begin = usedFerries.begin();
        FerrySet::const_iterator end = usedFerries.upper_bound(timeRemaining);
        if (end == usedFerries.begin()) {
            std::cerr << "Cannot remove more ferries.\n";
            return false;
        }
        auto iterator = std::max_element(begin, end,
                [](const Ferry* lhs, const Ferry* rhs) {
                    return lhs->skippedBikeTime < rhs->skippedBikeTime;
                });
        const Ferry* ferry = *iterator;
        std::cerr << "Removing ferry from " << problem.cityNames[ferry->from]
                << " to " << problem.cityNames[ferry->to] << "\n";
        usedFerries.erase(iterator);
        bikeTime += ferry->skippedBikeTime;
        totalTime += ferry->skippedBikeTime - ferry->time;
        return true;
    }

    class CompareFerries {
    public:
        bool operator()(const Ferry* lhs, const Ferry* rhs) const {
            return getKey(lhs) < getKey(rhs);
        }

        bool operator()(int lhs, const Ferry* rhs) const {
            return lhs < getKey(rhs);
        }

        bool operator()(const Ferry* lhs, int rhs) const {
            return getKey(lhs) < rhs;
        }

        using is_transparent = int;

    private:
        int getKey(const Ferry* ferry) const {
            return ferry->skippedBikeTime - ferry->time;
        }
    };

    Problem problem;
    using FerrySet = std::multiset<const Ferry*, CompareFerries>;
    FerrySet usedFerries;
    int bikeTime;
    int totalTime;
};

int main() {
    Solver solver{readInput(std::cin)};
    solver.solve();
    auto solution = solver.getResult();
    std::cout << solution.size();
    for (const auto& ferry : solution) {
        std::cout << ferry.first << " " << ferry.second << "\n";
    }
}
