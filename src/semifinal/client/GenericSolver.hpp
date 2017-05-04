#ifndef SEMIFINAL_CLIENT_GENERICSOLVER_HPP
#define SEMIFINAL_CLIENT_GENERICSOLVER_HPP

#include "GameState.hpp"

template<typename Strategy>
class GenericSolver {
public:
    explicit GenericSolver(Strategy strategy) : strategy(std::move(strategy)) {}

    void login(const std::vector<std::string>& loginInfos) {
        parseLogin(loginInfos);
    }

    void init() {
        std::cerr << "Resetting gameState\n";
        gameState = nullptr;
        strategy.reset();
    }

    std::vector<std::string> process(
            const std::vector<std::string>& tickInfos) {
        if (gameState == nullptr) {
            gameState = std::make_unique<GameState>(parseInitial(tickInfos));
            strategy.setGameState(*gameState);
            std::cerr << "Number of players (init): " << gameState->gameInfo.numPlayers
                    << "\n";
            return {};
        }
        auto error = parseTickInfo(*gameState, tickInfos);
        if (error) {
            std::cerr << "Error str: " << *error << std::endl;
            if (error->find("Not sent PUSH") != std::string::npos) {
                std::cerr << "Skipping message because of prev timeout\n";
                return {};
            }
        }

        std::cerr << "Number of players (step): " << gameState->gameInfo.numPlayers
                << "\n";
        std::cout << "Player " << gameState->activePlayerId << "\n"
                << toBox(gameState->track, gameState->gameInfo.playerId,
                           gameState->ourTurn() ? gameState->targetMonitor : -1,
                           gameState->gameInfo.blocked);
        if (gameState->ourTurn()) {
            auto step = strategy.ourTurn(*gameState);
            std::cout << "Step: " << step << "\n";
            auto trackAfter = gameState->track;
            executeStep(trackAfter, gameState->gameInfo.playerId, step);
            std::cout << toBox(trackAfter,
                               gameState->gameInfo.playerId,
                               gameState->targetMonitor,
                               gameState->gameInfo.blocked);

            return createOutput(gameState->track, step, gameState->ourPosition());
        } else {
            strategy.opponentsTurn(gameState->track, gameState->activePlayerId);
            return {};
        }
    }

    void end(const std::vector<std::string>& endInfos) {
        std::string message = parseGameEnd(endInfos);
        std::cerr << "Game over: " << message << std::endl;
        gameState = nullptr;
    }

private:
    Strategy strategy;
    std::unique_ptr<GameState> gameState;
};



#endif // SEMIFINAL_CLIENT_GENERICSOLVER_HPP
