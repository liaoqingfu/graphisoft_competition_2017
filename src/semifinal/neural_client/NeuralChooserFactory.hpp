#ifndef NEURAL_CLIENT_NEURALCHOOSERFACTORY_HPP
#define NEURAL_CLIENT_NEURALCHOOSERFACTORY_HPP

#include <IChooser.hpp>
#include <RandomChooser.hpp>
#include <BestChooser.hpp>
#include <MaxReachableChooser.hpp>
#include <PrincessMovingChooser.hpp>
#include <MonitorDefendingChooser.hpp>
#include <LookaheadChooser.hpp>

#include <GameState.hpp>

#include <NeuronWeights.hpp>

#include <memory>

//============================================================================//

template<typename NeuralNetwork>
class NeuralChooserFactory {
private:

public:
    explicit NeuralChooserFactory(std::mt19937& rng,
            NeuralNetwork neuralNetwork)
            : rng(rng),
              neuralNetwork(std::move(neuralNetwork)) {
//               neuralNetwork(hiddenLayerCount,
//                       neuronPerHiddenLayer,
//                       inputNeuronCount,
//                       outputNeuronCount) {
        // assert(learningParams.inputNeuronCount == inputNeronCount);
        // assert(learningParams.outputNeuronCount == outputNeronCount);
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
        int lookaheadDepth = 1;
        int lookaheadWeight = result[0]; // 4;
        int monitorReachabilityWeight = result[1]; // 10;
        int areaReachabilityWeight = result[2]; // 2;
        int princessMovingWeight = result[3]; // 10;
        bool princessMovingOverride = false;
        int maxReachableWeight = result[4]; // 2;
        return createChoosers(lookaheadDepth, lookaheadWeight,
                monitorReachabilityWeight, areaReachabilityWeight,
                princessMovingWeight, princessMovingOverride,
                maxReachableWeight);
    }

private:
    std::unique_ptr<LookaheadChooser> createChoosers(int lookaheadDepth,
            int lookaheadWeight, int monitorReachabilityWeight,
            int areaReachabilityWeight, int princessMovingWeight,
            bool princessMovingOverride, int maxReachableWeight) {
        return std::make_unique<LookaheadChooser>(
                c<MonitorDefendingChooser>(
                        c<PrincessMovingChooser>(
                                c<MaxReachableChooser>(
                                        c<BestChooser>(c<RandomChooser>(rng)),
                                        maxReachableWeight),
                                princessMovingWeight, princessMovingOverride),
                        monitorReachabilityWeight, areaReachabilityWeight),
                lookaheadDepth, lookaheadWeight);
    }

    template<typename T, typename...Args>
    std::shared_ptr<T> c(Args&&...args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
    std::mt19937& rng;
    NeuralNetwork neuralNetwork;
};

//============================================================================//

#endif // NEURAL_CLIENT_NEURALCHOOSERFACTORY_HPP
