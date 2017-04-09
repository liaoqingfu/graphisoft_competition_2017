#include "Lake.hpp"
#include "MultiNeuralNetwork.hpp"
#include "Neural.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    assert(argc >= 2);
    LearningParameters parameters;
    parameters.inputNeuronCount = NeuralFerryChooser::inputNeuronCount;
    parameters.outputNeuronCount = NeuralFerryChooser::outputNeuronCount;
    NeuralFerryChooser ferryChooser{parameters};
    {
        MultiNeuralNetwork<2> network;
        std::ifstream input(argv[1]);
        boost::archive::text_iarchive ar{input};
        ar >> network;
        ferryChooser.setNeuralNetwork(std::move(network));
    }

    int iterationLimit = 20;
    if (argc > 2) {
        iterationLimit = boost::lexical_cast<std::size_t>(argv[2]);
    }
    Solver<NeuralFerryChooser> solver{readInput(std::cin), ferryChooser,
            iterationLimit};
    solver.findShortestPath();
    solver.solve();
    auto solution = solver.getResult();
    std::cout << solution.size() << "\n";
    for (const auto& ferry : solution) {
        std::cout << ferry.first << " " << ferry.second << "\n";
    }
    solver.checkResult();
}

