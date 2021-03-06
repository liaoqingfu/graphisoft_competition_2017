#include "NeuralChooserFactory.hpp"

#include <client.hpp>
#include <AssemblingChooser.hpp>
#include <ChoosingStrategy.hpp>
#include <Debug.hpp>
#include <GenericSolver.hpp>
#include <StrategyParser.hpp>

#include <MultiNeuralNetwork.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <memory>
#include <random>

struct Options {
    std::string hostname = "localhost";
    unsigned short port = 32323;
    int level = 1;
    std::string teamName = "elvont_gyar";
    std::string password = "T&kX,!RT;vXK";
    unsigned seed = 0;
    std::string strategyString = "";
    std::string aiFile = "best.ai";
    bool debug;
};

namespace po = boost::program_options;

template <typename T>
po::typed_value<T>* defaultValue(T& v) {
    return po::value(&v)->default_value(v);
}


Options parseOptions(int argc, const char* argv[]) {
    po::options_description description;
    Options options;
    bool isHelpRequested = false;
    description.add_options()
            ("help,h", po::bool_switch(&isHelpRequested))
            ("port,P", defaultValue(options.port))
            ("host,H", defaultValue(options.hostname))
            ("task-id,t", defaultValue(options.level))
            ("team-name", defaultValue(options.teamName))
            ("password", defaultValue(options.password))
            ("seed,S", defaultValue(options.seed))
            ("strategy,s", po::value(&options.strategyString))
            ("ai-file,a", defaultValue(options.aiFile))
            ("debug,d", po::bool_switch(&options.debug))
            ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
            options(description). run(), vm);
    po::notify(vm);


    if (isHelpRequested) {
        std::cerr << description << std::endl;
        exit(1);
    }

    return options;
}

int main(int argc, const char* argv[]) {
    Options options = parseOptions(argc, argv);
    debugEnabled = options.debug;
    if (options.seed == 0) {
        options.seed = std::random_device{}();
    }

    try {
        platform_dep::enable_socket _;
        std::shared_ptr<std::mt19937> rng = std::make_shared<std::mt19937>(
                options.seed);

        using Solver = GenericSolver<ChoosingStrategy>;
        std::unique_ptr<Solver> solver;
        if (options.strategyString != "") {
            solver = std::make_unique<Solver>(
                            parseStrategy(options.strategyString, *rng));
        } else {
            std::cerr << "Neural network is used!" << std::endl;
            using ChooserFactory = NeuralChooserFactory<MazeNeuralNetwork>;
            using Chooser = AssemblingChooser<ChooserFactory>;
            ChooserFactory factory{rng};
            factory.setNeuralNetwork(getNeuralNetwork<MazeNeuralNetwork>(options.aiFile));
            solver = std::make_unique<Solver>(ChoosingStrategy(
                            std::make_shared<Chooser>(factory)));
        }
        client<Solver> mazeClient{options.hostname.c_str(), options.port,
                    options.teamName.c_str(), options.password.c_str(),
                    options.level, std::move(*solver)};
        mazeClient.run();
    } catch (std::exception& error) {
        std::cerr << "Exception thrown: " << error.what() << std::endl;
    }
}
