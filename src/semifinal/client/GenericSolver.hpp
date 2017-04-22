#ifndef SEMIFINAL_CLIENT_GENERICSOLVER_HPP
#define SEMIFINAL_CLIENT_GENERICSOLVER_HPP

#include "GameState.hpp"

template<typename Strategy>
class GenericSolver {
public:
    GenericSolver(Strategy strategy) : strategy(std::move(strategy)) {}

    void init(const std::vector<std::string>& fieldInfos) {
        gameState = GameState(parseInitial(fieldInfos));
    }

    std::vector<std::string> process(
            const std::vector<std::string>& tickInfos) {
        parseTickInfo(gameState, tickInfos);

        if (gameState.ourTurn()) {
            return createOutput(strategy(gameState), gameState.ourPosition());
        } else {
            return {};
        }
    }

    void end(const std::string& message) {
        std::cerr << "Game over: " << message << std::endl;
    }

private:
    Strategy strategy;
    GameState gameState;
};



#endif // SEMIFINAL_CLIENT_GENERICSOLVER_HPP
