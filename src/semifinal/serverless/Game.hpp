#ifndef SEMIFINAL_SERVERLESS_GAME_HPP
#define SEMIFINAL_SERVERLESS_GAME_HPP

#include "ChoosingStrategy.hpp"
#include "GameState.hpp"
#include "Options.hpp"
#include "Random.hpp"


#include <atomic>
#include <time.h>
#include <memory>
#include <vector>

struct Game : public std::enable_shared_from_this<Game> {
public:
    Game(Rng& rng, Options options,
            const std::vector<ChoosingStrategy>& strategies);
    void run(bool print);
    void setRng(Rng& rng) { this->rng = &rng; }
    void printScores() const;
private:
    struct Score {
        std::atomic<int> score{0};
        std::atomic<clock_t> time{0};

        Score() = default;
        Score(const Score&) = delete;
        Score& operator=(const Score&) = delete;
        Score(Score&&) = delete;
        Score& operator=(Score&&) = delete;

    };

    struct PlayerState {
        PlayerState(ChoosingStrategy strategy, int playerId) :
                strategy{std::move(strategy)},
                score{std::make_shared<Score>()} {
            gameState.gameInfo.playerId = playerId;
        }

        PlayerState(const PlayerState&) = default;
        PlayerState& operator=(const PlayerState&) = default;

        PlayerState(PlayerState&&) noexcept = default;
        PlayerState& operator=(PlayerState&&) noexcept = default;

        ChoosingStrategy strategy;
        GameState gameState;
        std::shared_ptr<Score> score;
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
