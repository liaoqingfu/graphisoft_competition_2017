#include "ChoosingStrategy.hpp"
#include "Game.hpp"
#include "StrategyParser.hpp"
#include "Options.hpp"
#include "Random.hpp"

#include <thread>
#include <iostream>

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

    Game game{rng, options, createStrategies(rng, options)};

    if (options.jobs == 1) {
        for (int i = 0; i < options.numRuns; ++i) {
            std::cout << "Run #" << i << "\n";
            game.run(options.numRuns == 1);
        }
    } else {
        std::vector<std::unique_ptr<std::thread>> threads;
        for (std::size_t i = 0; i < options.jobs; ++i) {
            threads.push_back(std::make_unique<std::thread>(
                    [game, &options, i, seed=rng()]() mutable {
                        Rng rng{seed};
                        game.setRng(rng);
                        for (int j = i; j < options.numRuns;
                                j += options.jobs) {
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
    game.printScores();
}
