#include "ChoosingStrategy.hpp"
#include "Game.hpp"
#include "StrategyParser.hpp"
#include "Options.hpp"
#include "PotentialStep.hpp"
#include "Random.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <thread>
#include <iostream>
#include <thread>

std::vector<ChoosingStrategy> createStrategies(Rng& rng,
        const Options& options) {
    std::vector<ChoosingStrategy> result;
    for (const std::string& strategyString : options.strategyStrings) {
        result.push_back(parseStrategy(strategyString, rng));
    }
    return result;
}

int main(int argc, const char* argv[]) {
    Options options = parseOptions(argc, argv);
    if (options.seed == 0) {
        options.seed = std::random_device{}();
    }
    Rng rng{options.seed};
    debugEnabled = options.debug;

    std::vector<std::shared_ptr<Score>> scores;
    for (int i = 0; i < options.numPlayers; ++i) {
        scores.push_back(std::make_shared<Score>());
    }

    if (options.jobs == 1) {

        for (int i = 0; i < options.numRuns; ++i) {
            std::cout << "Run #" << i << "\n";
            Game game{rng, options.width, options.height, options.numDisplays,
                        options.maxTick, options.numPlayers, options.blocked,
                        createStrategies(rng, options),
                        scores};
            game.run(options.numRuns == 1);
        }
    } else {
        std::vector<std::unique_ptr<std::thread>> threads;
        for (std::size_t i = 0; i < options.jobs; ++i) {
            threads.push_back(std::make_unique<std::thread>(
                    [&options, i, seed=rng(), &scores]() mutable {
                        Rng rng{seed};
                        for (int j = i; j < options.numRuns;
                                j += options.jobs) {
                            Game game{rng, options.width, options.height,
                                        options.numDisplays, options.maxTick,
                                        options.numPlayers, options.blocked,
                                    createStrategies(rng, options), scores};
                            game.run(false);
                            std::cout << ".";
                            std::cout.flush();
                        }
                    }));
        }
        for (const auto& thread : threads) {
            thread->join();
        }
        std::cout << "\n";
    }

    std::cout << "Game over.\n";
    for (int i = 0; i < options.numPlayers; ++i) {
        std::cout << setColor(defaultColor, playerColors[i])
                << "Player " << i << " final score "
                << scores[i]->score << " Total time spent: "
                << scores[i]->time << clearColor() << std::endl;
    }
}
