
#include "NeuralNetwork.hpp"
#include <cassert>
#include <cmath>
#include <iomanip>

NeuralNetwork::NeuralNetwork(
        unsigned hiddenLayerCount,
        unsigned hiddenLayerNeuronCount,
        unsigned inputNeuronCount,
        unsigned outputNeuronCount) :
            hiddenLayerCount(hiddenLayerCount),
            hiddenLayerNeuronCount(hiddenLayerNeuronCount),
            inputNeuronCount(inputNeuronCount),
            outputNeuronCount(outputNeuronCount),
            weights(getWeightCountForNetwork(hiddenLayerCount,
                    hiddenLayerNeuronCount, inputNeuronCount, outputNeuronCount),
                    0.0f) {
}

unsigned NeuralNetwork::getWeightCountForNetwork(
        unsigned hiddenLayerCount,
        unsigned hiddenLayerNeuronCount,
        unsigned inputNeuronCount,
        unsigned outputNeuronCount) {
    return (hiddenLayerCount == 0) ?
                (outputNeuronCount * (inputNeuronCount + 1)) :
                (hiddenLayerNeuronCount * (inputNeuronCount + outputNeuronCount +
                        (hiddenLayerCount - 1) * hiddenLayerNeuronCount) +
                    (hiddenLayerCount * hiddenLayerNeuronCount + outputNeuronCount));
}

Weights NeuralNetwork::evaluateInput(Weights input, std::ostream& /*os*/) {
    assert(input.size() == inputNeuronCount);

    Weights output;

    unsigned weightIndex = 0;
    for (unsigned layer = 0; layer <= hiddenLayerCount; ++layer) {
        // os << "  layer " << layer << "\n";
        unsigned neuronCount = (layer == hiddenLayerCount) ?
                outputNeuronCount :
                hiddenLayerNeuronCount;

        output.clear();
        output.reserve(neuronCount);
        for (unsigned neuron = 0; neuron < neuronCount; ++neuron) {
            // os << "    neuron " << neuron << "\n      ";
            Weight netInput = 0;

            for (auto value: input) {
                Weight w = weights[weightIndex++];
                // os << std::setw(10) << value << "*" << std::setw(10) << w << " + ";
                netInput += w*value;
            }
            Weight w = weights[weightIndex++];
            netInput += -1.f * w;

            Weight sigmoid = sigmoidApproximation(netInput);
            // os << "-1*" << std::setw(10) << w << " = " << netInput << " [" << sigmoid << "]\n";
            output.push_back(sigmoid);

        }
        input = output; //we could move here probably
    }
    return output;
}

std::string NeuralNetwork::getExternalParameter(const std::string& key) const {
    auto it = externalParameters.find(key);
    return it == externalParameters.end() ? "" : it->second;
}

void NeuralNetwork::setExternalParameter(std::string key, std::string value) {
    externalParameters[std::move(key)] = std::move(value);
}
