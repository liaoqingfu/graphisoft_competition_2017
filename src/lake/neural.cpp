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
    ferryChooser.setNeuralNetwork(getNeuralNetwork());

    Solver<NeuralFerryChooser> solver{readInput(std::cin), ferryChooser};
    solver.findShortestPath();
    if (solver.getBestTotalTime() > solver.getProblem().timeLimit) {
        std::cout << "0" << std::endl;
        std::cerr << "There is no solution.\n";
        throwError(Error::NoSolution);
    } else {
        solver.solve();
        auto solution = solver.getResult();
        std::cout << solution.size() << "\n";
        for (const auto& ferry : solution) {
            std::cout << ferry.first << " " << ferry.second << "\n";
        }
        if (!solver.checkResult()) {
            throwError(Error::WrongSolution);
        }
    }
    return 0;
}
