#include "Lake.hpp"
#include "Neural.hpp"
#include "NeuralNetworkData.hpp"

#include <iostream>
#include <sstream>

int main() {
    LearningParameters parameters;
    parameters.inputNeuronCount = NeuralFerryChooser::inputNeuronCount;
    parameters.outputNeuronCount = NeuralFerryChooser::outputNeuronCount;
    NeuralFerryChooser ferryChooser{parameters};
    ferryChooser.setNeuralNetwork(getNeuralNetworkData());

    Solver<NeuralFerryChooser> solver{readInput(std::cin), ferryChooser};
    solver.findShortestPath();
    solver.solve();
    auto solution = solver.getResult();
    std::cout << solution.size() << "\n";
    for (const auto& ferry : solution) {
        std::cout << ferry.first << " " << ferry.second << "\n";
    }
}
