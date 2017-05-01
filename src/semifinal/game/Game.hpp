#ifndef SEMIFINAL_SERVERLESS_GAME_HPP
#define SEMIFINAL_SERVERLESS_GAME_HPP

#include "ChoosingStrategy.hpp"
#include "GameState.hpp"

#include <boost/date_time/posix_time/posix_time_duration.hpp>

#include <atomic>
#include <memory>
#include <mutex>
#include <random>
#include <vector>

//============================================================================//

struct Score {
    std::atomic<int> score{0};
    boost::posix_time::time_duration time = boost::posix_time::seconds(0);

    std::mutex mutex;

    Score() = default;
    Score(const Score&) = delete;
    Score& operator=(const Score&) = delete;
    Score(Score&&) = delete;
    Score& operator=(Score&&) = delete;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

struct Game : public std::enable_shared_from_this<Game> {
public:
    Game(std::mt19937& rng, int width, int height, int numDisplays, int maxTick,
            const std::vector<ChoosingStrategy>& strategies,
            const std::vector<std::shared_ptr<Score>>& scores);

    void run(bool print);

private:
    struct PlayerState {
        PlayerState(ChoosingStrategy strategy, int playerId,
                const std::shared_ptr<Score>& score) :
                strategy{std::move(strategy)},
                score{score} {
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

    std::mt19937* rng;
    int width;
    int height;
    int numDisplays;
    int maxTick;
    std::vector<PlayerState> playerStates;
};

//============================================================================//

#endif // SEMIFINAL_SERVERLESS_GAME_HPP
