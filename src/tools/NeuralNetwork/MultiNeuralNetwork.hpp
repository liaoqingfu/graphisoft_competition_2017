#ifndef TOOLS_NEURALNETWORK_MULTINEURALNETWORK_HPP
#define TOOLS_NEURALNETWORK_MULTINEURALNETWORK_HPP

#include "NeuralNetwork.hpp"

#include <array>

template<std::size_t N>
class MultiNeuralNetwork {
public:
    MultiNeuralNetwork() = default;

    MultiNeuralNetwork(
            unsigned hiddenLayerCount,
            unsigned hiddenLayerNeuronCount,
            unsigned inputNeuronCount,
            unsigned outputNeuronCount) {
        for (NeuralNetwork& network : neuralNetworks) {
            network = NeuralNetwork{
                    hiddenLayerCount,
                    hiddenLayerNeuronCount,
                    inputNeuronCount,
                    outputNeuronCount};
        }
    }

    static unsigned getWeightCountForNetwork(
            unsigned hiddenLayerCount,
            unsigned hiddenLayerNeuronCount,
            unsigned inputNeuronCount,
            unsigned outputNeuronCount) {
        return N * NeuralNetwork::getWeightCountForNetwork(
                hiddenLayerCount,
                hiddenLayerNeuronCount,
                inputNeuronCount,
                outputNeuronCount);
    }

    NeuralNetwork& get(std::size_t i) {
        return neuralNetworks[i];
    }

    void setWeights(const Weights& weights) {
        auto begin = weights.begin();
        for (NeuralNetwork& network : neuralNetworks) {
            auto size = network.getWeightCount();
            Weights w(size);
            auto end = begin + size;
            std::copy(begin, end, w.begin());
            network.setWeights(std::move(w));
            begin = end;
        }
    }

private:
    std::array<NeuralNetwork, N> neuralNetworks;
};


#endif // TOOLS_NEURALNETWORK_MULTINEURALNETWORK_HPP
