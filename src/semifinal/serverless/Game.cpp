#include "Game.hpp"

#include <set>

Game::Game(Rng& rng, Options options,
        const std::vector<ChoosingStrategy>& strategies,
        const std::vector<std::shared_ptr<Score>>& scores) :
        rng(&rng), options(std::move(options)) {
    assert(scores.size() == numPlayers);
    for (int i = 0; i < static_cast<int>(numPlayers); ++i) {
        playerStates.emplace_back(strategies[i % strategies.size()], i,
                scores[i]);
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
        playerStates[i].strategy.init(state.gameInfo);
    }

    for (gameState.currentTick = 0;
         gameState.currentTick < gameState.gameInfo.maxTick &&
         gameState.track.getRemainingMonitors() != 0;
         ++gameState.currentTick) {
        for (PlayerState& playerState : playerStates) {
            int playerId = playerState.gameState.gameInfo.playerId;
            int targetMonitor = playerState.gameState.targetMonitor;
            Track& track = gameState.track;
            int extraField = playerState.gameState.extraField;
            const OpponentsInfo& opponentsInfo =
                playerState.strategy.getOpponentsInfo();

            if (print) {
                std::cerr << "Tick " << gameState.currentTick << " monitors "
                          << track.getRemainingMonitors() << "\n"
                          << setColor(defaultColor, playerColors[playerId])
                          << "Player " << playerId << " score "
                          << playerState.score->score << " target M" << targetMonitor
                          << track.getMonitor(targetMonitor)
                          << " extraField  "
                          << extraField
                          << clearColor()
                          << "\n"
                          << toBox(gameState.track, playerId, targetMonitor);
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
                    actualPlayer.strategy.opponentsTurn(
                        playerState.gameState.track, playerId);
                }
                clock_t end = ::clock();
                actualPlayer.score->time += end - start;

            }

            for (PlayerState& actualPlayer : playerStates) {
                if (actualPlayer.strategy.getOpponentsInfo()[playerId]
                        .targetMonitors.count(targetMonitor) == 0) {
                    std::cerr << "ERR with target monitors\n";
                }
            }

            if (print) {
                std::cerr << "Step: " << step << "\n"
                          << "opponentsInfo:\n" << opponentsInfo;
            }

            playerState.gameState.extraField = executeStep(track, playerId,
                    step);

            auto debugOpponentsInfo = [&]() {
                for (int i = 0; i < (int)opponentsInfo.size(); ++i) {
                    if (i == playerId) continue;
                    const auto& targets = opponentsInfo[i].targetMonitors;
                    auto alivesNum =
                        playerState.gameState.track.getAliveMonitors().size();
                    if (targets.size() <= 2) {
                        std::cerr << "Targets less than two, player" << playerId
                                  << " for opp" << i << " \n";
                    }
                    if (targets.size() * 2 <= alivesNum) {
                        std::cerr << "Targets at half size, player" << playerId
                                  << " for opp" << i << " \n";
                    }
                    if (targets.size() < alivesNum * 0.8) {
                        std::cerr << "Targets at 80 percent size for opp" << i
                                  << " \n";
                        std::cerr << "Targets at 80 percent size, player"
                                  << playerId << " for opp" << i << " \n";
                    }
                }
            };
            debugOpponentsInfo();
            for (const auto& ps : playerStates) {
                std::cerr << "OppInfo of player"
                          << ps.gameState.gameInfo.playerId << "\n";
                std::cerr << ps.strategy.getOpponentsInfo();
            }

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
