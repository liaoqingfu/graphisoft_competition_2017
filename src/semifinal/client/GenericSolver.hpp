#ifndef SEMIFINAL_CLIENT_GENERICSOLVER_HPP
#define SEMIFINAL_CLIENT_GENERICSOLVER_HPP

#include "GameState.hpp"

template<typename Strategy>
class GenericSolver {
public:
    GenericSolver(Strategy strategy) : strategy(std::move(strategy)) {}

    void init(const std::vector<std::string>& fieldInfos) {
        gameState = parseInitialGameState(fieldInfos);
    }

    std::vector<std::string> process(
            const std::vector<std::string>& tickInfos) {
        parseTickInfo(gameState, tickInfos);
        return createOutput(strategy(gameState));
    }

    void end(const std::string& message) {
        std::cerr << "Game over: " << message << std::endl;
    }

private:
    Strategy strategy;
    GameState gameState;
};



#endif // SEMIFINAL_CLIENT_GENERICSOLVER_HPP
