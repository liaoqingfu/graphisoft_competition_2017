#include "NeuralChooserFactory.hpp"

#include <client.hpp>
#include <AssemblingChooser.hpp>
#include <ChoosingStrategy.hpp>
#include <Debug.hpp>
#include <Game.hpp>
#include <GenericSolver.hpp>
#include <StrategyParser.hpp>

#include <LearningController.hpp>
#include <LearningParameters.hpp>
#include <MultiNeuralNetwork.hpp>

#include <util/ThreadPool.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <vector>

//============================================================================//

struct Options {
    unsigned seed = 0;
    std::vector<std::string> strategyStrings;
    int iterationLimit = 4;
    std::size_t numThreads = 1;
    LearningParameters learningParameters;
    bool debug;
};

//----------------------------------------------------------------------------//

namespace po = boost::program_options;

//----------------------------------------------------------------------------//

template <typename T>
po::typed_value<T>* defaultValue(T& v) {
    return po::value(&v)->default_value(v);
}

//----------------------------------------------------------------------------//

Options parseOptions(int argc, const char* argv[]) {
    po::options_description description;
    Options options;
    options.learningParameters.inputNeuronCount = 5;
    options.learningParameters.outputNeuronCount = 5;
    description.add_options()
            ("help,h", "Show this help.")
            ("seed,S", defaultValue(options.seed))
            ("strategy,s", po::value(&options.strategyStrings)->multitoken())
            ("hidden-layer-count",
             defaultValue(options.learningParameters.hiddenLayerCount),
             "Hidden layer count")
            ("neuron-per-hidden-layer",
             defaultValue(options.learningParameters.neuronPerHiddenLayer),
             "Neurons per hidden layer")
            ("population-size",
             defaultValue(options.learningParameters.populationSize),
             "The size of the population")
            ("generation-limit",
             defaultValue(options.learningParameters.generationLimit),
             "Stop after this many generations")
            ("printout-frequency",
             defaultValue(options.learningParameters.printoutFrequency),
             "Print diagnostic after each n generations")
            ("best-ai-file",
             defaultValue(options.learningParameters.bestAIFile),
             "The file to save the best neural network")
            ("population-input-file,i",
             defaultValue(options.learningParameters.populationInputFile),
             "Load initial population from here")
            ("population-output-file,o",
             defaultValue(options.learningParameters.populationOutputFile),
             "The file to save the best neural network")
            ("starting-populations",
             defaultValue(options.learningParameters.startingPopulations),
             "The number of independent populations to start the learning "
             "with.")
            ("population-cutoff",
             defaultValue(options.learningParameters.populationCutoff),
             "The number of generations after the worst population is dropped "
             "(if there are more than one).")
            ("iteration-limit,n",
             defaultValue(options.iterationLimit),
             "The iteration limit for the solver.")
            ("threads,j",
             defaultValue(options.numThreads),
             "The number of threads to use.")
            ("debug", po::bool_switch(&options.debug))
            ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
            options(description).run(), vm);
    po::notify(vm);

    if (vm.count("help") != 0) {
        std::cerr << description << std::endl;
        exit(1);
    }

    return options;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

using MazeNeuralNetwork = MultiNeuralNetwork<1>;

//----------------------------------------------------------------------------//

MazeNeuralNetwork getNeuralNetwork() {
    static constexpr unsigned inputNeuronCount = 5;
    static constexpr unsigned outputNeuronCount = 5;
    static constexpr unsigned hiddenLayerCount = 1;
    static constexpr unsigned neuronPerHiddenLayer = 60;

    MazeNeuralNetwork result{hiddenLayerCount,
                neuronPerHiddenLayer,
                inputNeuronCount,
                outputNeuronCount};
    return result;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

template<typename Factory>
std::vector<ChoosingStrategy> createStrategies(std::mt19937& rng,
        const std::vector<std::string>& strategyStrings, Factory factory) {
    std::vector<ChoosingStrategy> result;
    // Put neural chooser first, this will be player 0
    // TODO: don't assume neural player to be player 0
    using Chooser = AssemblingChooser<Factory>;
    result.push_back(ChoosingStrategy(std::make_shared<Chooser>(factory)));
    while (result.size() < 4) {
        for (const std::string& strategyString : strategyStrings) {
            result.push_back(parseStrategy(strategyString, rng));
        }
    }
    return result;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

struct GameParameters {
    int width;
    int height;
    int numDisplays;
    int maxTick;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

class GameManager {
public:
    GameManager(std::mt19937& rng, LearningParameters learningParams,
            GameParameters gameParams, std::vector<std::string> strategyStrings)
            : rng(rng),
              learningParams(std::move(learningParams)),
              gameParams(std::move(gameParams)),
              strategyStrings(std::move(strategyStrings)),
              chooserFactory(rng) {
        for (std::size_t i = 0; i < 4; ++i) {
            scores.push_back(std::make_shared<Score>());
        }
    }

    GameManager(const GameManager&) = delete;
    GameManager(GameManager&&) = default;
    GameManager& operator=(const GameManager&) = delete;
    GameManager& operator=(GameManager&&) = default;

    void setNeuralNetwork(MazeNeuralNetwork network) {
        chooserFactory.setNeuralNetwork(std::move(network));
    }

    void init() {
        // empty
    }

    void run() {
        std::vector<ChoosingStrategy> strategies = createStrategies(rng,
                strategyStrings,
                [this](const GameState& gameState) {
                    return this->chooserFactory(gameState);
                });
        Game game{rng, gameParams.width, gameParams.height,
                    gameParams.numDisplays, gameParams.maxTick,
                    strategies, scores};
        game.run(false);
    }

    float getFitness() const {
        // TODO include other players score
        return static_cast<float>(scores[0]->score)
                / gameParams.numDisplays;
    }

    std::string getDebugInfo() const {
        std::ostringstream ss;
        ss << "score=" << scores[0]->score;
        return ss.str();
    }

private:
    std::mt19937& rng;
    LearningParameters learningParams;
    GameParameters gameParams;
    std::vector<std::string> strategyStrings;
    NeuralChooserFactory<MazeNeuralNetwork> chooserFactory;
    std::vector<std::shared_ptr<Score>> scores;
};

int main(int argc, const char* argv[]) {
    Options options = parseOptions(argc, argv);
    debugEnabled = options.debug;
    assert(options.strategyStrings.size() > 0);
    if (options.seed == 0) {
        options.seed = std::random_device{}();
    }

    try {
        std::mt19937 rng{options.seed};

        std::vector<GameParameters> gameParams;
        std::uniform_int_distribution<> sizeDistribution(6, 15);
        for (int i = 0; i < options.iterationLimit; ++i) {
            int height = sizeDistribution(rng);
            int width = sizeDistribution(rng);
            int numDisplays = std::uniform_int_distribution<>(
                    4, width*height/4)(rng);
            int maxTick = std::uniform_int_distribution<>(
                    numDisplays, 2*numDisplays)(rng);
            gameParams.push_back(GameParameters{height, width, numDisplays,
                                maxTick});
        }

        util::ThreadPool threadPool{options.numThreads};
        threadPool.start();
        boost::asio::io_service& ioService = threadPool.getIoService();
        LearningController<GameParameters> learningController{
            options.learningParameters, gameParams, ioService};
        learningController.run<MazeNeuralNetwork>(
                [&options, &rng](const GameParameters& gameParams) {
                    return GameManager{rng, options.learningParameters,
                                gameParams, options.strategyStrings};
                });

        threadPool.wait();
    } catch (std::exception& error) {
        std::cerr << "Exception thrown: " << error.what() << std::endl;
    }
}

//============================================================================//
