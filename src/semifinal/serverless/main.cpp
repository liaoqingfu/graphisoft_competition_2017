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

    std::vector<std::shared_ptr<Score>> scores;
    for (std::size_t i = 0; i < numPlayers; ++i) {
        scores.push_back(std::make_shared<Score>());
    }

    if (options.jobs == 1) {

        for (int i = 0; i < options.numRuns; ++i) {
            std::cout << "Run #" << i << "\n";
            Game game{rng, options, createStrategies(rng, options), scores};
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
                            Game game{rng, options, 
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
    for (std::size_t i = 0; i < numPlayers; ++i) {
        std::cout << setColor(defaultColor, playerColors[i])
                << "Player " << i << " final score "
                << scores[i]->score << " Total time spent: "
                << static_cast<double>(scores[i]->time) / CLOCKS_PER_SEC
                << " s" << clearColor() << std::endl;
    }
}
