#include "client.hpp"
#include "ChoosingStrategy.hpp"
#include "GenericSolver.hpp"
#include "RandomChooser.hpp"

#include <boost/lexical_cast.hpp>

#include <iostream>

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0]
                << " host port task_id [seed]";
    }
    const char* host_name = argv[1];
    const auto port = boost::lexical_cast<unsigned short>(argv[2]);
    const int task_id = boost::lexical_cast<int>(argv[3]);
    const char* team_name = "elvont_gyar";
    const char* password = "T&kX,!RT;vXK";


    try {
        platform_dep::enable_socket _;

        using Solver = GenericSolver<ChoosingStrategy>;

        unsigned int seed;
        if (argc > 4) {
            seed = boost::lexical_cast<int>(argv[4]);
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
