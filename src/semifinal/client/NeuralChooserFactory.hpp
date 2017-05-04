#ifndef NEURAL_CLIENT_NEURALCHOOSERFACTORY_HPP
#define NEURAL_CLIENT_NEURALCHOOSERFACTORY_HPP

#include "IChooser.hpp"
#include "BestChooser.hpp"
#include "LookaheadChooser.hpp"
#include "MaxReachableChooser.hpp"
#include "MonitorDefendingChooser.hpp"
#include "OpponentPushCheckingChooser.hpp"
#include "PrincessMovingChooser.hpp"
#include "RandomChooser.hpp"

#include <GameState.hpp>

#include <MultiNeuralNetwork.hpp>
#include <NeuronWeights.hpp>

#include <boost/archive/text_iarchive.hpp>

#include <memory>
#include <random>

//============================================================================//

using MazeNeuralNetwork = MultiNeuralNetwork<1>;

template<typename NeuralNetwork>
class NeuralChooserFactory {
private:

public:
    explicit NeuralChooserFactory(std::shared_ptr<std::mt19937> rng)
            : rng(std::move(rng)) {
//               neuralNetwork(hiddenLayerCount,
//                       neuronPerHiddenLayer,
//                       inputNeuronCount,
//                       outputNeuronCount) {
        // assert(learningParams.inputNeuronCount == inputNeronCount);
        // assert(learningParams.outputNeuronCount == outputNeronCount);
    }

    NeuralChooserFactory(std::shared_ptr<std::mt19937> rng,
            NeuralNetwork neuralNetwork)
            : rng(std::move(rng)),
              neuralNetwork(std::move(neuralNetwork)) {
    }

    void setRng(std::shared_ptr<std::mt19937> rng) {
        this->rng = std::move(rng);
    }

    void setNeuralNetwork(NeuralNetwork neuralNetwork) {
        this->neuralNetwork = std::move(neuralNetwork);
    }

    std::unique_ptr<IChooser> operator()(const GameState& gameState) {
        int N = gameState.gameInfo.height;
        int M = gameState.gameInfo.width;
        Weights inputs{
            (N + M) / 30.0f,
            N * M / 225.0f,
            gameState.gameInfo.numDisplays / N * M / 4.0f,
            gameState.track.getRemainingMonitors() / N * M / 4.0f,
            (gameState.gameInfo.maxTick - gameState.currentTick) /
                    static_cast<float>(gameState.gameInfo.maxTick)
        };
        std::vector<float> result = neuralNetwork.get(0).evaluateInput(inputs);
        //assert(result.size() == outputNeuronCount);
        LookaheadType lookaheadType = LookaheadType::normal;
        float lookaheadWeight = result[0]; // 4;
        float opponentPushCheckingWeight = result[1];
        float monitorReachabilityWeight = result[2]; // 10;
        float areaReachabilityWeight = result[3]; // 2;
        float princessMovingWeight = result[4]; // 10;
        bool princessMovingOverride = false;
        float maxReachableWeight = result[5]; // 2;
        return createChoosers(lookaheadType, lookaheadWeight,
                opponentPushCheckingWeight, monitorReachabilityWeight,
                areaReachabilityWeight, princessMovingWeight,
                princessMovingOverride, maxReachableWeight);
    }

private:
    std::unique_ptr<LookaheadChooser> createChoosers(
            LookaheadType lookaheadType,
            float lookaheadWeight, float opponentPushCheckingWeight,
            float monitorReachabilityWeight, float areaReachabilityWeight,
            float princessMovingWeight, bool princessMovingOverride,
            float maxReachableWeight) {
        return std::make_unique<LookaheadChooser>(c<OpponentPushCheckingChooser>(
                c<MonitorDefendingChooser>(
                        c<PrincessMovingChooser>(
                                c<MaxReachableChooser>(
                                        c<BestChooser>(c<RandomChooser>(*rng)),
                                        maxReachableWeight),
                                princessMovingWeight, princessMovingOverride),
                        monitorReachabilityWeight, areaReachabilityWeight),
                opponentPushCheckingWeight), lookaheadType, lookaheadWeight);
    }

    template<typename T, typename...Args>
    std::shared_ptr<T> c(Args&&...args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    std::shared_ptr<std::mt19937> rng;
    NeuralNetwork neuralNetwork;
};

template<typename NeuralNetwork>
NeuralNetwork getNeuralNetwork(std::string aiFile) {
    NeuralNetwork result;
    std::ifstream input{aiFile};
    boost::archive::text_iarchive ar{input};
    ar >> result;
    return result;
}

//============================================================================//

#endif // NEURAL_CLIENT_NEURALCHOOSERFACTORY_HPP
