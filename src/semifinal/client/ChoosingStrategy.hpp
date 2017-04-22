#ifndef SEMIFINAL_CLIENT_CHOOSINGSTRATEGY_HPP
#define SEMIFINAL_CLIENT_CHOOSINGSTRATEGY_HPP

#include "IChooser.hpp"
#include "GameState.hpp"
#include "OpponentsInfo.hpp"

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
        updateOpponentsInfo(gameState.track, gameState.gameInfo.playerId);
        return calculateStep();
    }

    void opponentsTurn(const Track& track, int playerId);

private:
    Step calculateStep();

    friend struct OpponentsInfoTestFixture;
    void updateOpponentsInfo(const Track& track, int playerId);

    std::shared_ptr<IChooser> chooser;
    GameState gameState;

    OpponentsInfo opponentsInfo;

    struct PrevState {
        Track track;
        int playerId = -1;
    } prevSt;
};

#endif // SEMIFINAL_CLIENT_CHOOSINGSTRATEGY_HPP
