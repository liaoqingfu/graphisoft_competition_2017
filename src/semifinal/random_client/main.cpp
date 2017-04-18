#include "client.hpp"
#include "ChoosingStrategy.hpp"
#include "GenericSolver.hpp"
#include "RandomChooser.hpp"

#include <boost/lexical_cast.hpp>

#include <iostream>

int main(int argc, char** argv) {
    if (argc < 6) {
        std::cerr << "Usage: " << argv[0]
                << " host port team_name password task_id [seed]";
    }
    const char* host_name = argv[1];
    const auto port = boost::lexical_cast<unsigned short>(argv[2]);
    const char* team_name = argv[3];
    const char* password = argv[4];

    const int task_id = boost::lexical_cast<int>(argv[5]);

    try {
        platform_dep::enable_socket _;

        using Solver = GenericSolver<ChoosingStrategy>;

        unsigned int seed;
        if (argc > 6) {
            seed = boost::lexical_cast<int>(argv[6]);
        } else {
            seed = std::random_device{}();
        }
        std::mt19937 rng{seed};

        Solver solver{ChoosingStrategy{std::make_unique<RandomChooser>(rng)}};
        client<Solver>(host_name, port, team_name,
                password, task_id, std::move(solver)).run();

    } catch(std::exception& e) {
        std::cerr << "Exception thrown. what(): " << e.what() << std::endl;
    }
}
