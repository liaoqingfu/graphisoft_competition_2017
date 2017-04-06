#include "LearningController.hpp"
#include "LearningParameters.hpp"
#include "MultiNeuralNetwork.hpp"

#include <util/ThreadPool.hpp>
#include <type_traits>

struct GameInfo {};

using Network = MultiNeuralNetwork<3>;

class GameManager {
public:
    void init() {}
    void run() {}
    void setNeuralNetwork(const Network&) {}
    float getFitness() const { return 0; }
    std::string getDebugInfo() const { return ""; }
};

GameManager createGameManager(const GameInfo&) {
    return GameManager{};
}

int main() {
    util::ThreadPool threadPool{1};
    threadPool.start();

    LearningParameters parameters;
    parameters.inputNeuronCount = 1;
    parameters.outputNeuronCount = 1;
    parameters.generationLimit = 10;
    LearningController<GameInfo> learningController{parameters, {GameInfo{}},
            threadPool.getIoService()};
    learningController.run<Network>(createGameManager);
}
