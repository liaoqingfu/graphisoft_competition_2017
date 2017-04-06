#include "MultiNeuralNetwork.hpp"

#include <boost/archive/text_iarchive.hpp>

#include <fstream>
#include <iomanip>

int main(int argc, char* argv[]) {
    assert(argc == 3);
    MultiNeuralNetwork<2> network;
    {
        std::ifstream input(argv[1]);
        boost::archive::text_iarchive ar{input};
        ar >> network;
    }

    const NeuralNetwork& network0 = network.get(0);
    const NeuralNetwork& network1 = network.get(1);
    std::ofstream output{argv[2]};
    output << R"(
#ifndef NEURAL_NETWORK_DATA_HPP
#define NEURAL_NETWORK_DATA_HPP

#include "MultiNeuralNetwork.hpp"

MultiNeuralNetwork<2> getNeuralNetwork() {
    MultiNeuralNetwork<2> result{)"
            << network0.getHiddenLayerCount() << ", "
            << network0.getHiddenLayerNeuronCount() << ", "
            << network0.getInputNeuronCount() << ", "
            << network0.getOutputNeuronCount() << R"(};
    result.get(0).setWeights({)";
    output << std::setprecision(20);
    for (float weight : network0.getWeights()) {
        output << weight << ",";
    }
    output << R"(});
    result.get(1).setWeights({)";
    for (float weight : network1.getWeights()) {
        output << weight << ",";
    }
    output << R"(});
    return result;
}

#endif
)";
}
