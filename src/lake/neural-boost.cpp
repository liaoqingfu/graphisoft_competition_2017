#include "Lake.hpp"
#include "MultiNeuralNetwork.hpp"
#include "Neural.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    assert(argc == 2);
    LearningParameters parameters;
    parameters.inputNeuronCount = NeuralFerryChooser::inputNeuronCount;
    parameters.outputNeuronCount = NeuralFerryChooser::outputNeuronCount;
    NeuralFerryChooser ferryChooser{parameters, std::cerr};
    {
        MultiNeuralNetwork<2> network;
        std::ifstream input(argv[1]);
        boost::archive::text_iarchive ar{input};
        ar >> network;
        ferryChooser.setNeuralNetwork(std::move(network));
    }

    {
        boost::archive::text_oarchive ar{std::cerr};
        ar << ferryChooser.getNeuralNetwork();
    }
    std::cerr << "\n";

    Solver<NeuralFerryChooser> solver{readInput(std::cin), ferryChooser,
            std::cerr};
    solver.findShortestPath();
    solver.solve();
    auto solution = solver.getResult();
    std::cout << solution.size() << "\n";
    for (const auto& ferry : solution) {
        std::cout << ferry.first << " " << ferry.second << "\n";
    }
    solver.checkResult();
}

