#ifndef TOOLS_NEURALNETWORK_MULTINEURALNETWORK_HPP
#define TOOLS_NEURALNETWORK_MULTINEURALNETWORK_HPP

#include "NeuralNetwork.hpp"

#include <vector>

class MultiNeuralNetwork {
public:
    NeuralNetwork(
            unsigned hiddenLayerCount,
            unsigned hiddenLayerNeuronCount,
            unsigned inputNeuronCount,
            unsigned outputNeuronCount);

    static unsigned getWeightCountForNetwork(
            unsigned hiddenLayerCount,
            unsigned hiddenLayerNeuronCount,
            unsigned inputNeuronCount,
            unsigned outputNeuronCount);

private:
    std::vector<NeuralNetworrk> neuralNetworks;
};


#endif // TOOLS_NEURALNETWORK_MULTINEURALNETWORK_HPP
