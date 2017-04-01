#include "Field.hpp"
#include "HexMatrix.hpp"

#include <boost/program_options.hpp>
#include <iostream>
#include <random>

namespace po = boost::program_options;

struct Options {
    int halfWidth = 10;
    int height = 10;
    int numCorridors = 10;
    int numMonitors = 10;
    float stopProbability = 0.1;
    float turnProbability = 0.2;
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
        ("half-width,W", defaultValue(options.halfWidth)) ("height,H", defaultValue(options.height))
        ("corridors,c", defaultValue(options.numCorridors))
        ("monitors,m", defaultValue(options.numMonitors))
        ("stop-probability,s", defaultValue(options.stopProbability))
        ("turn-probability,t", defaultValue(options.turnProbability))
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

class Generator {
public:
    Generator(Options options) :
            options(std::move(options)),
            rng(this->options.seed == 0 ? std::random_device{}() :
                    this->options.seed) {
    }

    Matrix<Field> generate() {
        matrix.reset(options.halfWidth * 2,
                options.height, Field::wall);
        for (int i = 0; i < options.numCorridors; ++i) {
            createCorridor();
        }
        for (int i = 0; i < options.numMonitors; ++i) {
            createMonitor();
        }
        return matrix;
    }

private:
    void createCorridor() {
        Point p{xDistribution(rng), yDistribution(rng)};
        do {
            int direction = directionDistribution(rng);
            do {
                matrix[p] = Field::corridor;
                p += hex::getNeighbors(p)[direction];
                if (!isInsideMatrix(matrix, p)) {
                    return;
                }
            } while (!turnDistribution(rng));
        } while (!stopDistribution(rng));
    }

    void createMonitor() {
        while (true) {
            Point p{xDistribution(rng), yDistribution(rng)};
            if (matrix[p] == Field::corridor) {
                matrix[p] = Field::monitor;
                return;
            }
        }
    }

    Options options;
    std::mt19937 rng;
    Matrix<Field> matrix;
    std::uniform_int_distribution<int> xDistribution{0,
            options.halfWidth * 2 - 1};
    std::uniform_int_distribution<int> yDistribution{0, options.height - 1};
    std::uniform_int_distribution<int> directionDistribution{0,
            hex::numNeighbors - 1};
    std::bernoulli_distribution stopDistribution{options.stopProbability};
    std::bernoulli_distribution turnDistribution{options.turnProbability};
};

int main(int argc, char* argv[]) {
    auto matrix = Generator{parseOptions(argc, argv)}.generate();
    hex::printMatrix(std::cerr, matrix);
    hex::printEcosim(std::cout, matrix);
}
