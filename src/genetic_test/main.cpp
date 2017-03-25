#include "LearningController.hpp"
#include "LearningParameters.hpp"

#include <util/ThreadPool.hpp>
#include <type_traits>

struct GameInfo {};

class GameManager {
public:
    void init() {}
    void run() {}
    float getFitness() { return 0; }
};

GameManager createGameManager(const GameInfo&) {
    return GameManager{};
}

template<typename T>
void check(T) {
    static_assert(std::is_same<GameManager,
            typename std::result_of<T(GameInfo)>::type>::value,"");
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
    // learningController.run(createGameManager);
    check(createGameManager);
}
