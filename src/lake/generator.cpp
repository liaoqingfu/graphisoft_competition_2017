#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

namespace po = boost::program_options;

struct Options {
    int numCities = 10;
    int numFerries = 10;
    int timeLimit = 100;
    int minCycleTime = 10;
    int maxCycleTime = 60;
    int minFerryTime = 10;
    int maxFerryTime = 30;
    int seed = 0;
};

template <typename T>
po::typed_value<T>* defaultValue(T& v) {
    return po::value(&v)->default_value(v);
}

Options parseOptions(int argc, char* argv[]) {
    po::options_description optionsDescription;
    Options options;
    bool help = false;
    optionsDescription.add_options()
        ("help,h", po::bool_switch(&help))
        ("cities", defaultValue(options.numCities))
        ("ferries", defaultValue(options.numFerries))
        ("time-limit", defaultValue(options.timeLimit))
        ("min-cycle-time", defaultValue(options.minCycleTime))
        ("max-cycle-time", defaultValue(options.maxCycleTime))
        ("min-ferry-time", defaultValue(options.minFerryTime))
        ("max-ferry-time", defaultValue(options.maxFerryTime))
        ("seed", defaultValue(options.seed))
        ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
            options(optionsDescription).
            run(), vm);
    po::notify(vm);

    if (help) {
        std::cerr << optionsDescription << "\n";
        exit(0);
    }
    return options;
}

struct Ferry {
    int from;
    int to;
    int time;
};

struct Result {
    std::vector<int> cycleTimes;
    std::vector<Ferry> ferries;
};

class Generator {
public:
    Generator(Options options) :
            options(std::move(options)),
            rng(this->options.seed == 0 ? std::random_device{}() :
                    this->options.seed) {
    }

    Result generate() {
        result.cycleTimes.reserve(options.numCities);
        std::generate_n(std::back_inserter(result.cycleTimes),
                options.numCities,
                [this]() { return cycleTimeDistribution(rng); });

        result.ferries.reserve(options.numFerries);
        std::generate_n(std::back_inserter(result.ferries), options.numFerries,
                [this]() { return generateFerry(); });
        return result;
    }

private:
    Ferry generateFerry() {
        Ferry ferry;
        do {
            std::uniform_int_distribution<int> fromDistribution{0,
                    options.numCities - 2};
            ferry.from = fromDistribution(rng);

            std::uniform_int_distribution<int> toDistribution{ferry.from + 1,
                    options.numCities - 1};
            ferry.to = toDistribution(rng);

            ferry.time = ferryTimeDistribution(rng);
        } while (!isAcceptableFerry(ferry) || isDuplicateFerry(ferry));
        return ferry;
    }

    bool isDuplicateFerry(const Ferry& ferry) {
        return std::find_if(result.ferries.begin(), result.ferries.end(),
                [this, ferry](const Ferry& other) {
                    return ferry.from == other.from && ferry.to == other.to;
                }) != result.ferries.end();
    }

    bool isAcceptableFerry(const Ferry& ferry) {
        auto begin = result.cycleTimes.begin();
        return std::accumulate(begin + ferry.from, begin + ferry.to, 0) >
                ferry.time;
    }

    Options options;
    std::mt19937 rng;
    std::uniform_int_distribution<int> cycleTimeDistribution{
            options.minCycleTime, options.maxCycleTime};
    std::uniform_int_distribution<int> ferryTimeDistribution{
            options.minFerryTime, options.maxFerryTime};
    Result result;
};

std::string getName(int i) {
    if (i == 0) {
        return "Graphiváros";
    } else {
        return "Graphi" + boost::lexical_cast<std::string>(i);
    }
}

int main(int argc, char* argv[]) {
    auto options = parseOptions(argc, argv);
    auto result = Generator{options}.generate();

    std::cout << options.numCities << "\n";
    for (int i = 0; i < options.numCities; ++i) {
        std::cout << getName(i) << "\n";
    }
    for (int cycleTime : result.cycleTimes) {
        std::cout << cycleTime << " ";
    }
    std::cout << "\n" << result.ferries.size() << "\n";
    for (const Ferry ferry : result.ferries) {
        std::cout << getName(ferry.from) << " " << getName(ferry.to) <<
                " " << ferry.time << "\n";
    }
    std::cout << options.timeLimit << "\n";
}
