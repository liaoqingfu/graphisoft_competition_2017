#ifndef SEMIFINAL_SERVERLESS_GAME_HPP
#define SEMIFINAL_SERVERLESS_GAME_HPP

#include "ChoosingStrategy.hpp"
#include "GameState.hpp"
#include "Options.hpp"
#include "Random.hpp"

#include <time.h>
#include <vector>

struct Game {
public:
    Game(Rng& rng, Options options,
            const std::vector<ChoosingStrategy>& strategies);
    void run(bool print);
    void setRng(Rng& rng) { this->rng = &rng; }
    void printScores() const;
private:
    struct PlayerState {
        PlayerState(ChoosingStrategy strategy) : strategy{std::move(strategy)} {
        }

        PlayerState(const PlayerState&) = delete;
        PlayerState& operator=(const PlayerState&) = delete;

        PlayerState(PlayerState&&) noexcept = default;
        PlayerState& operator=(PlayerState&&) noexcept = default;

        ChoosingStrategy strategy;
        GameState gameState;
        int score = 0;
        clock_t time = 0;
    };

    GameState generateGame();
    void setPlayerMonitors(GameState& globalState);
    int getRandomMonitor(const GameState& gameState);
    std::vector<Point> generatePoints(int width, int height,
            std::size_t number);

    Rng* rng;
    Options options;
    std::vector<PlayerState> playerStates;
};


#endif // SEMIFINAL_SERVERLESS_GAME_HPP
