#ifndef SEMIFINAL_CLIENT_CHOOSINGSTRATEGY_HPP
#define SEMIFINAL_CLIENT_CHOOSINGSTRATEGY_HPP

#include "IChooser.hpp"
#include "GameState.hpp"

#include <memory>

class ChoosingStrategy {
public:
    ChoosingStrategy() = default;

    ChoosingStrategy(std::shared_ptr<IChooser> chooser) :
            chooser(std::move(chooser)) {}

    ChoosingStrategy(const ChoosingStrategy&) = default;
    ChoosingStrategy& operator=(const ChoosingStrategy&) = default;

    ChoosingStrategy(ChoosingStrategy&&) noexcept = default;
    ChoosingStrategy& operator=(ChoosingStrategy&&) = default;

    Step ourTurn(GameState gameState) {
        this->gameState = std::move(gameState);
        return calculateStep();
    }

    void opponentsTurn(const Track& /*track*/, int /*playerId*/) {
        // TODO implement
    }

private:
    Step calculateStep();

    std::shared_ptr<IChooser> chooser;
    GameState gameState;
};

#endif // SEMIFINAL_CLIENT_CHOOSINGSTRATEGY_HPP
