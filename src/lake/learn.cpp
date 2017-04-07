#include "Lake.hpp"
#include "LearningController.hpp"
#include "LearningParameters.hpp"
#include "MultiNeuralNetwork.hpp"
#include "Neural.hpp"

#include "util/ThreadPool.hpp"

#include <boost/archive/text_oarchive.hpp>
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

    GameManager(LearningParameters parameters, Problem problem) :
            parameters(std::move(parameters)),
            debugStream(std::make_unique<std::ostringstream>()),
            ferryChooser(std::make_unique<NeuralFerryChooser>(
                    this->parameters, *debugStream)),
            solverTemplate(std::move(problem), *ferryChooser,
                    *debugStream) {
        solverTemplate.findShortestPath();
    }

    GameManager(const GameManager&) = delete;
    GameManager(GameManager&&) = default;
    GameManager& operator=(const GameManager&) = delete;
    GameManager& operator=(GameManager&&) = default;

    void setNeuralNetwork(Network network) {
        ferryChooser->setNeuralNetwork(std::move(network));
    }

    void init() {
        solver = std::make_unique<Solver>(solverTemplate);
    }

    void run() {
        solver->solve();
        std::ostringstream filename;
        filename << "debug_" << id << "_" << solver->getBestBikeTime() <<
                ".log";
        std::ofstream fs{filename.str()};

        {
            boost::archive::text_oarchive ar{fs};
            ar << ferryChooser->getNeuralNetwork();
        }

        fs << "\n" << debugStream->str();
        debugStream->str("");
    }

    float getFitness() const {
        return static_cast<float>(solver->getBestBikeTime())
            / solver->getProblem().timeLimit;
    }

    std::string getDebugInfo() const {
        std::ostringstream ss;
        ss << "time=" << solver->getBestTotalTime()
                << " bike time=" << solver->getBestBikeTime();
        return ss.str();
    }

private:
    static int maxId;
    int id{maxId++};
    LearningParameters parameters;
    std::unique_ptr<std::ostringstream> debugStream;
    std::unique_ptr<NeuralFerryChooser> ferryChooser;
    Solver solverTemplate;
    std::unique_ptr<Solver> solver;
};

int GameManager::maxId = 0;

struct Options {
    std::vector<std::string> inputFileName;
    std::size_t numThreads = 1;
    LearningParameters learningParameters;
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
                return GameManager(options.learningParameters, problem);
            });
    threadPool.wait();
}
