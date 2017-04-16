#include "client.hpp"
#include "HeuristicSolver.hpp"
#include "RandomChooser.hpp"

#include <iostream>

int main(int argc, char** argv) {
    if (argc != 6) {
        std::cerr << "Usage: " << argv[0]
                << " host port team_name password task_id";
    }
    const char* host_name = argv[1];
    const auto port = boost::lexical_cast<unsigned short>(argv[2]);
    const char* team_name = argv[3];
    const char* password = argv[4];

    const int task_id = boost::lexical_cast<int>(argv[5]);

    try {
        platform_dep::enable_socket _;

        HeuristicSolver<RandomChooser> solver{RandomChooser{}};
        client<HeuristicSolver<RandomChooser>>(host_name, port, team_name,
                password, task_id, std::move(solver)).run();

    } catch(std::exception& e) {
        std::cerr << "Exception throwed. what(): " << e.what() << std::endl;
    }
}
