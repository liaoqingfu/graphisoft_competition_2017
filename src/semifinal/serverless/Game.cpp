#include "Game.hpp"

#include <set>

Game::Game(Rng& rng, Options options,
    const std::vector<ChoosingStrategy>& strategies) :
        rng(&rng), options(std::move(options)) {
    for (int i = 0; i < static_cast<int>(numPlayers); ++i) {
        playerStates.emplace_back(strategies[i % strategies.size()], i);
    }

}

void Game::setPlayerMonitors(GameState& globalState) {
    for (PlayerState& playerState : playerStates) {
        if (globalState.track.getMonitor(playerState.gameState.targetMonitor)
                .x < 0) {
            playerState.gameState.targetMonitor = getRandomMonitor(globalState);
        }
    }
}

int Game::getRandomMonitor(const GameState& gameState) {
    std::uniform_int_distribution<int> distribution{0,
            gameState.gameInfo.numDisplays - 1};
    int result;
    do {
        result = distribution(*rng);
    } while (gameState.track.getMonitor(result).x < 0);
    return result;
}

std::vector<Point> Game::generatePoints(int width, int height,
        std::size_t number) {
    std::set<Point> result;
    std::uniform_int_distribution<int> xDistribution{0, width - 1};
    std::uniform_int_distribution<int> yDistribution{0, height - 1};

    while (result.size() != number) {
        Point p{xDistribution(*rng), yDistribution(*rng)};
        result.insert(p);
    }
    return std::vector<Point>(result.begin(), result.end());
}


GameState Game::generateGame() {
    GameState result;
    auto& gi = result.gameInfo;
    gi.width = options.width;
    gi.height = options.height;
    gi.numDisplays = options.numDisplays;
    gi.maxTick = options.maxTick;
    result.currentTick = 0;
    result.extraField = 15;

    std::uniform_int_distribution<int> fieldDistribution{1, 15};
    int numFields = gi.width * gi.height;
    std::vector<int> fields;
    fields.reserve(numFields);
    for (int i = 0; i < numFields; ++i) {
        fields.push_back(fieldDistribution(*rng));
    }

    auto monitors =
            generatePoints(gi.width, gi.height,
                    gi.numDisplays);
    std::cerr << "Monitors: ";
    for (Point p : monitors) {
        std::cerr << p << " ";
    }
    std::cerr << "\n";

    result.track = Track(gi.width, gi.height, fields, monitors,
            generatePoints(gi.width, gi.height, numPlayers));

    return result;
}

void Game::run(bool print) {
    GameState gameState = generateGame();
    for (std::size_t i = 0; i < playerStates.size(); ++i) {
        GameState& state = playerStates[i].gameState;
        state = gameState;
        state.gameInfo.playerId = i;
        state.targetMonitor = getRandomMonitor(state);
    }

    for (gameState.currentTick = 0;
         gameState.currentTick < gameState.gameInfo.maxTick &&
         gameState.track.getRemainingMonitors() != 0;
         ++gameState.currentTick) {
        for (PlayerState& playerState : playerStates) {
            int playerId = playerState.gameState.gameInfo.playerId;
            int targetMonitor = playerState.gameState.targetMonitor;
            Track& track = gameState.track;

            if (print) {
                std::cerr
                        << "Tick " << gameState.currentTick
                        << " monitors " << track.getRemainingMonitors()
                        << "\n" << setColor(defaultColor,
                                playerColors[playerId])
                        << "Player " << playerId
                        << " score " << playerState.score->score
                        << " target " << track.getMonitor(targetMonitor)
                        << clearColor()
                        << "\n" << toBox(gameState.track, playerId,
                                targetMonitor);
            }

            playerState.gameState.track = track;
            playerState.gameState.currentTick = gameState.currentTick;

            Step step;
            for (PlayerState& actualPlayer : playerStates) {
                clock_t start = ::clock();
                if (actualPlayer.gameState.gameInfo.playerId
                        == playerState.gameState.gameInfo.playerId) {
                    step = actualPlayer.strategy.ourTurn(
                            actualPlayer.gameState);
                } else {
                    actualPlayer.strategy.opponentsTurn(track,
                            actualPlayer.gameState.gameInfo.playerId);
                }
                clock_t end = ::clock();
                actualPlayer.score->time += end - start;
            }
            if (print) {
                std::cerr << "Step: " << step << "\n";
            }
            playerState.gameState.extraField = executeStep(track, playerId,
                    step);

            if (track.getPrincess(playerId) ==
                    track.getMonitor(targetMonitor)) {
                ++playerState.score->score;
                if (print) {
                    std::cerr << "Monitor removed: " << targetMonitor
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
                setPlayerMonitors(gameState);
            }
            if (print) {
                std::cerr << "\n" << std::endl;
            }
        }
    }
}

void Game::printScores() const {
    for (const PlayerState& playerState : playerStates) {
        int playerId = playerState.gameState.gameInfo.playerId;
        std::cout << setColor(defaultColor, playerColors[playerId])
                << "Player " << playerId << " final score "
                << playerState.score->score << " Total time spent: "
                << static_cast<double>(playerState.score->time) / CLOCKS_PER_SEC
                << " s" << clearColor() << std::endl;
    }
}
