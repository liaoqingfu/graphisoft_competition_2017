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
    int gameCount = 4;
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
    options.learningParameters.outputNeuronCount = 6;
    options.learningParameters.neuronPerHiddenLayer = 15;
    options.learningParameters.populationSize = 20;
    options.learningParameters.populationCutoff = 4;
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
            ("game-count,n",
             defaultValue(options.gameCount),
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
    GameManager(unsigned seed, LearningParameters learningParams,
            GameParameters gameParams, std::vector<std::string> strategyStrings)
            : seed(seed / learningParams.populationSize),
              rng(std::make_shared<std::mt19937>(this->seed)),
              learningParams(std::move(learningParams)),
              gameParams(std::move(gameParams)),
              strategyStrings(std::move(strategyStrings)),
              chooserFactory(rng) {
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
        scores.clear();
        for (std::size_t i = 0; i < 4; ++i) {
            scores.push_back(std::make_shared<Score>());
        }

        rng = std::make_shared<std::mt19937>(
                ++seed / learningParams.populationSize);
        chooserFactory.setRng(rng);
        std::vector<ChoosingStrategy> strategies = createStrategies(*rng,
                strategyStrings,
                [this](const GameState& gameState) {
                    return this->chooserFactory(gameState);
                });

        // std::uniform_int_distribution<> sizeDistribution(6, 15);
        // int height = sizeDistribution(rng);
        // int width = sizeDistribution(rng);
        // int numDisplays = std::uniform_int_distribution<>(
        //         4, width*height/4)(rng);
        // int maxTick = std::uniform_int_distribution<>(
        //         numDisplays, 2*numDisplays)(rng);
        // gameParams = GameParameters{width, height, numDisplays, maxTick};
        Game game{*rng, gameParams.width, gameParams.height,
                    gameParams.numDisplays, gameParams.maxTick,
                    strategies, scores};
        game.run(false);
    }

    float getFitness() const {
        // TODO include other players score
        return static_cast<float>(scores[0]->score);
        //        / gameParams.numDisplays;
    }

    std::string getDebugInfo() const {
        std::ostringstream ss;
        ss << "score=" << scores[0]->score;
        return ss.str();
    }

private:
    unsigned seed;
    std::shared_ptr<std::mt19937> rng;
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
        for (int i = 0; i < options.gameCount; ++i) {
            int height = sizeDistribution(rng);
            int width = sizeDistribution(rng);
            int numDisplays = std::uniform_int_distribution<>(
                    4, width*height/4)(rng);
            int maxTick = std::uniform_int_distribution<>(
                    numDisplays, 2*numDisplays)(rng);
            gameParams.push_back(GameParameters{height, width, numDisplays,
                                maxTick});
            // game params: h: 12 w: 8 d: 8 t: 10
            // game params: h: 10 w: 8 d: 12 t: 18
            // game params: h: 6 w: 8 d: 7 t: 13
            // game params: h: 11 w: 11 d: 23 t: 26
            // game params: h: 15 w: 14 d: 18 t: 19

            std::cerr << "game params: h: " << height << " w: " << width
                      << " d: " << numDisplays << " t: " << maxTick << std::endl;
        }

        gameParams.clear();
        gameParams.push_back(GameParameters{12, 8, 8, 10});
        gameParams.push_back(GameParameters{10, 8, 12, 18});
        gameParams.push_back(GameParameters{6, 8, 7, 13});
        gameParams.push_back(GameParameters{11, 11, 23, 26});
        gameParams.push_back(GameParameters{15, 14, 18, 19});

        util::ThreadPool threadPool{options.numThreads};
        threadPool.start();
        boost::asio::io_service& ioService = threadPool.getIoService();
        unsigned seed = std::uniform_int_distribution<>{}(rng);
        LearningController<GameParameters> learningController{
            options.learningParameters, gameParams, ioService};
        learningController.run<MazeNeuralNetwork>(
                [&options, &seed](const GameParameters& gameParams) {
                    return GameManager{seed, options.learningParameters,
                                gameParams, options.strategyStrings};
                });

        threadPool.wait();
    } catch (std::exception& error) {
        std::cerr << "Exception thrown: " << error.what() << std::endl;
    }
}

//============================================================================//
