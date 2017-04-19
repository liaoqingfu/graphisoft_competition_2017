#include "ChoosingStrategy.hpp"
#include "GameState.hpp"
#include "LookaheadChooser.hpp"
#include "PrincessMovingChooser.hpp"
#include "BestChooser.hpp"
#include "RandomChooser.hpp"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <random>
#include <set>
#include <time.h>

using Rng = std::mt19937;

std::vector<Point> generatePoints(Rng& rng, int width, int height,
        std::size_t number) {
    std::set<Point> result;
    std::uniform_int_distribution<int> xDistribution{0, width - 1};
    std::uniform_int_distribution<int> yDistribution{0, height - 1};

    while (result.size() != number) {
        Point p{xDistribution(rng), yDistribution(rng)};
        result.insert(p);
    }
    return std::vector<Point>(result.begin(), result.end());
}

GameState generateGame(Rng& rng) {
    GameState result;
    result.width = 15;
    result.height = 6;
    result.numDisplays = 20;
    result.maxTick = 40;
    result.currentTick = 0;
    result.extraField = 15;

    std::uniform_int_distribution<int> fieldDistribution{1, 15};
    int numFields = result.width * result.height;
    std::vector<int> fields;
    fields.reserve(numFields);
    for (int i = 0; i < numFields; ++i) {
        fields.push_back(fieldDistribution(rng));
    }

    auto monitors =
            generatePoints(rng, result.width, result.height,
                    result.numDisplays);
    std::cerr << "Monitors: ";
    for (Point p : monitors) {
        std::cerr << p << " ";
    }
    std::cerr << "\n";

    result.track = Track(result.width, result.height, fields, monitors,
            generatePoints(rng, result.width, result.height, numPlayers));

    return result;
}

int getRandomMonitor(Rng& rng, const GameState& gameState) {
    std::uniform_int_distribution<int> distribution{0,
            gameState.numDisplays - 1};
    int result;
    do {
        result = distribution(rng);
    } while (gameState.track.getMonitor(result).x < 0);
    return result;
}

struct PlayerState {
    PlayerState(ChoosingStrategy&& strategy) : strategy{std::move(strategy)} {}

    PlayerState(const PlayerState&) = delete;
    PlayerState& operator=(const PlayerState&) = delete;

    PlayerState(PlayerState&&) noexcept = default;
    PlayerState& operator=(PlayerState&&) noexcept = default;


    ChoosingStrategy strategy;
    GameState gameState;
    int score = 0;
    clock_t time = 0;
};

void setPlayerMonitors(Rng& rng, std::vector<PlayerState>& playerStates,
        GameState& globalState) {
    for (PlayerState& playerState : playerStates) {
        if (globalState.track.getMonitor(playerState.gameState.targetMonitor)
                .x < 0) {
            playerState.gameState.targetMonitor = getRandomMonitor(
                    rng, globalState);
        }
    }
}

std::vector<ChoosingStrategy> createStrategies(Rng& rng) {
    std::vector<ChoosingStrategy> result;
    result.emplace_back(std::make_unique<RandomChooser>(rng));
    result.emplace_back(std::make_unique<LookaheadChooser>(
            std::make_unique<BestChooser>(std::make_unique<RandomChooser>(rng)),
            1.0));
    result.emplace_back(std::make_unique<PrincessMovingChooser>(
            std::make_unique<BestChooser>(std::make_unique<RandomChooser>(rng)),
            1.0));
    result.emplace_back(std::make_unique<LookaheadChooser>(
            std::make_unique<PrincessMovingChooser>(
                    std::make_unique<BestChooser>(
                            std::make_unique<RandomChooser>(rng)),
                    1.0),
            1000.0));
    return result;
}

void runGame(Rng& rng, std::vector<PlayerState>& playerStates, bool print) {
    GameState gameState = generateGame(rng);
    for (std::size_t i = 0; i < playerStates.size(); ++i) {
        GameState& state = playerStates[i].gameState;
        state = gameState;
        state.playerId = i;
        state.targetMonitor = getRandomMonitor(rng, state);
    }

    for (gameState.currentTick = 0; gameState.currentTick < gameState.maxTick
            && gameState.track.getRemainingMonitors() != 0;
            ++gameState.currentTick) {
        for (PlayerState& playerState : playerStates) {
            int playerId = playerState.gameState.playerId;
            int targetMonitor = playerState.gameState.targetMonitor;
            Track& track = gameState.track;

            if (print) {
                std::cout
                        << "Tick " << gameState.currentTick
                        << " monitors " << track.getRemainingMonitors()
                        << "\n" << setColor(defaultColor,
                                playerColors[playerId])
                        << "Player " << playerId
                        << " score " << playerState.score
                        << " target " << track.getMonitor(targetMonitor)
                        << clearColor()
                        << "\n" << gameState.track;
            }

            playerState.gameState.track = track;
            playerState.gameState.currentTick = gameState.currentTick;

            clock_t start = ::clock();
            Step step = playerState.strategy(playerState.gameState);
            clock_t end = ::clock();
            playerState.time += end - start;
            if (print) {
                std::cout << "Step: push " << step.pushDirection
                        << " " << step.pushPosition
                        << " " << fieldTypes[step.pushFieldType]
                        << " princess=" << step.princessTarget << "\n";
            }
            playerState.gameState.extraField = executeStep(track, playerId,
                    step);

            if (track.getPrincess(playerId) ==
                    track.getMonitor(targetMonitor)) {
                ++playerState.score;
                if (print) {
                    std::cout << "Monitor removed: " << targetMonitor
                            << " " << track.getMonitor(targetMonitor)
                            << " Remaining: " << track.getRemainingMonitors()
                            << "\n"
                            << setColor(defaultColor, playerColors[playerId])
                            << "Score awarded to player " << playerId
                            << clearColor() << "\n";
                }
                track.removeMonitor(targetMonitor);
                if (track.getRemainingMonitors() == 0) {
                    break;
                }
                setPlayerMonitors(rng, playerStates, gameState);
            }
            if (print) {
                std::cout << "\n" << std::endl;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    unsigned int seed;
    if (argc > 1) {
        seed = boost::lexical_cast<int>(argv[1]);
    } else {
        seed = std::random_device{}();
    }
    Rng rng{seed};

    int numRuns = 1;
    if (argc > 2) {
        numRuns = boost::lexical_cast<int>(argv[2]);
    }

    std::vector<PlayerState> playerStates;

    {
        std::vector<ChoosingStrategy> strategies = createStrategies(rng);
        for (int i = 0; i < static_cast<int>(numPlayers); ++i) {
            playerStates.emplace_back(std::move(
                    strategies[i % strategies.size()]));
        }
    }

    for (int i = 0; i < numRuns; ++i) {
        std::cout << "Run #" << i << "\n";
        runGame(rng, playerStates, numRuns == 1);
    }

    std::cout << "Game over.\n";
    for (const PlayerState& playerState : playerStates) {
        int playerId = playerState.gameState.playerId;
        std::cout << setColor(defaultColor, playerColors[playerId])
                << "Player " << playerId << " final score "
                << playerState.score << " Total time spent: "
                << static_cast<double>(playerState.time) / CLOCKS_PER_SEC
                << " s" << clearColor() << std::endl;
    }
}
