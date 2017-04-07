#include "Lake.hpp"
#include "MultiNeuralNetwork.hpp"
#include "Neural.hpp"

#include <boost/archive/text_iarchive.hpp>

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    assert(argc == 2);
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

    Solver<NeuralFerryChooser> solver{readInput(std::cin), ferryChooser};
    solver.findShortestPath();
    solver.solve();
    auto solution = solver.getResult();
    std::cout << solution.size() << "\n";
    for (const auto& ferry : solution) {
        std::cout << ferry.first << " " << ferry.second << "\n";
    }
    solver.checkResult();
}

