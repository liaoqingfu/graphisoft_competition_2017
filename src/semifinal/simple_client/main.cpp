#include "client.hpp"
#include "ChoosingStrategy.hpp"
#include "GenericSolver.hpp"
#include "RandomChooser.hpp"
#include "StrategyParser.hpp"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <random>
#include <vector>

struct Options {
    std::string hostname;
    int port;
    int taskId;
    std::string teamName = "elvont_gyar";
    std::string password = "T&kX,!RT;vXK";
    unsigned seed = 2;
    std::string strategyString = "ChoosingStrategy("
        "LookaheadChooser(1,"
            "MonitorDefendingChooser("
                "PrincessMovingChooser("
                    "MaxReachableChooser("
                        "BestChooser(RandomChooser()),"
                    "2),"
                "10),"
            "10, 2),"
        "4))";
};

int main(int argc, const char* argv[]) {
    Options options;
    std::cerr << options.strategyString;
    if (argc <= 3) {
        std::cerr << "Usage: " << argv[0] << " host port task-id\n";
        return 1;
    }
    options.hostname = argv[1];
    options.port = boost::lexical_cast<int>(argv[2]);
    options.taskId = boost::lexical_cast<int>(argv[3]);

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
