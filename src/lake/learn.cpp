#include "Lake.hpp"
#include "LearningController.hpp"
#include "LearningParameters.hpp"
#include "MultiNeuralNetwork.hpp"
#include "Neural.hpp"

#include "util/ThreadPool.hpp"

#include <boost/program_options.hpp>

#include <algorithm>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class GameManager {
public:
    using Solver = ::Solver<NeuralFerryChooser>;

    GameManager(LearningParameters parameters, Problem problem,
            int iterationLimit) :
            parameters(std::move(parameters)),
            ferryChooser(std::make_unique<NeuralFerryChooser>(
                    this->parameters)),
            solverTemplate(std::move(problem), *ferryChooser, iterationLimit) {
    }

    GameManager(const GameManager&) = delete;
    GameManager(GameManager&&) = default;
    GameManager& operator=(const GameManager&) = delete;
    GameManager& operator=(GameManager&&) = default;

    void setNeuralNetwork(Network network) {
        ferryChooser->setNeuralNetwork(std::move(network));
    }

    void init() {
        if (!solverTemplate.isInitialized()) {
            solverTemplate.findShortestPath();
        }
        solver = std::make_unique<Solver>(solverTemplate);
        ferryChooser->initialize(*solver);
    }

    void run() {
        solver->solve();
        bestBikeTime = solver->getBestBikeTime();
        bestTotalTime = solver->getBestTotalTime();
        solver.reset();
    }

    float getFitness() const {
        return static_cast<float>(bestBikeTime)
                / solverTemplate.getProblem().timeLimit;
    }

    std::string getDebugInfo() const {
        std::ostringstream ss;
        ss << "time=" << bestTotalTime << " bike time=" << bestBikeTime;
        return ss.str();
    }

private:
    LearningParameters parameters;
    std::unique_ptr<NeuralFerryChooser> ferryChooser;
    Solver solverTemplate;
    std::unique_ptr<Solver> solver;
    int bestBikeTime = 0;
    int bestTotalTime = 0;
};

struct Options {
    std::vector<std::string> inputFileName;
    std::size_t numThreads = 1;
    LearningParameters learningParameters;
    int iterationLimit = 4;
};

template<typename T>
boost::program_options::typed_value<T>* defaultValue(T& value) {
    return boost::program_options::value(&value)->default_value(value);
}

Options parseOptions(int argc, const char* argv[]) {
    namespace po = boost::program_options;
    Options result;
    result.learningParameters.inputNeuronCount =
            NeuralFerryChooser::inputNeuronCount;
    result.learningParameters.outputNeuronCount =
            NeuralFerryChooser::outputNeuronCount;
    po::options_description optionsDescription;
    optionsDescription.add_options()
            ("help,h", "Show help.")
            ("hidden-layer-count",
             defaultValue(result.learningParameters.hiddenLayerCount),
             "Hidden layer count")
            ("neuron-per-hidden-layer",
             defaultValue(result.learningParameters.neuronPerHiddenLayer),
             "Neurons per hidden layer")
            ("population-size",
             defaultValue(result.learningParameters.populationSize),
             "The size of the population")
            ("generation-limit",
             defaultValue(result.learningParameters.generationLimit),
             "Stop after this many generations")
            ("printout-frequency",
             defaultValue(result.learningParameters.printoutFrequency),
             "Print diagnostic after each n generations")
            ("best-ai-file",
             defaultValue(result.learningParameters.bestAIFile),
             "The file to save the best neural network")
            ("population-input-file,i",
             defaultValue(result.learningParameters.populationInputFile),
             "Load initial population from here")
            ("population-output-file,o",
             defaultValue(result.learningParameters.populationOutputFile),
             "The file to save the best neural network")
            ("starting-populations",
             defaultValue(result.learningParameters.startingPopulations),
             "The number of independent populations to start the learning "
             "with.")
            ("population-cutoff",
             defaultValue(result.learningParameters.populationCutoff),
             "The number of generations after the worst population is dropped "
             "(if there are more than one).")
            ("input-files,I",
             po::value(&result.inputFileName)->multitoken(),
             "The input problems.")
            ("iteration-limit",
             defaultValue(result.iterationLimit),
             "The iteration limit for the solver.")
            ("threads,j",
             defaultValue(result.numThreads),
             "The number of threads to use.")
            ;
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
            options(optionsDescription).run(), vm);
    po::notify(vm);
    if (vm.count("help") != 0) {
        std::cerr << optionsDescription;
        std::exit(0);
    }
    return result;
}

int main(int argc, const char* argv[]) {
    Options options = parseOptions(argc, argv);
    std::vector<Problem> gameInfos(options.inputFileName.size());
    std::transform(options.inputFileName.begin(), options.inputFileName.end(),
            gameInfos.begin(),
            [](const std::string& filename) {
                std::ifstream stream{filename};
                return readInput(stream);
            });
    util::ThreadPool threadPool{options.numThreads};
    threadPool.start();
    boost::asio::io_service& ioService = threadPool.getIoService();
    LearningController<Problem> learningController{options.learningParameters,
            gameInfos, ioService};
    learningController.run<Network>(
            [&options](const Problem& problem) {
                return GameManager(options.learningParameters, problem,
                        options.iterationLimit);
            });
    threadPool.wait();
}
