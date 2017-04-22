#include "client.hpp"
#include "ChoosingStrategy.hpp"
#include "GenericSolver.hpp"
#include "RandomChooser.hpp"
#include "StrategyParser.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <random>
#include <vector>

struct Options {
    std::string hostname;
    int port;
    int taskId;
    std::string teamName = "elvont_gyar";
    std::string password = "T&kX,!RT;vXK";
    unsigned seed = 0;
    std::string strategyString;
};

namespace po = boost::program_options;

template <typename T>
po::typed_value<T>* defaultValue(T& v) {
    return po::value(&v)->default_value(v);
}

Options parseOptions(int argc, const char* argv[]) {
    po::options_description optionsDescription;
    Options options;
    bool help = false;
    optionsDescription.add_options()
        ("help,h", po::bool_switch(&help))
        ("port,P", defaultValue(options.port))
        ("host,H", defaultValue(options.hostname))
        ("task-id,t", defaultValue(options.taskId))
        ("team-name", defaultValue(options.teamName))
        ("password", defaultValue(options.password))
        ("seed,S", defaultValue(options.seed))
        ("strategy,s", po::value(&options.strategyString))
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
int main(int argc, const char* argv[]) {
    Options options = parseOptions(argc, argv);
    if (options.seed == 0) {
        options.seed = std::random_device{}();
    }

    try {
        platform_dep::enable_socket _;

        using Solver = GenericSolver<ChoosingStrategy>;

        std::mt19937 rng{options.seed};

        Solver solver{parseStrategy(options.strategyString, rng)};
        client<Solver>(options.hostname.c_str(), options.port,
                options.teamName.c_str(), options.password.c_str(),
                options.taskId, std::move(solver)).run();

    } catch(std::exception& e) {
        std::cerr << "Exception thrown. what(): " << e.what() << std::endl;
    }
}
