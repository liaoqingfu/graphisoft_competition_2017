#include "ChoosingStrategy.hpp"
#include "Game.hpp"
#include "StrategyParser.hpp"
#include "Options.hpp"
#include "Random.hpp"

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

    for (int i = 0; i < options.numRuns; ++i) {
        std::cout << "Run #" << i << "\n";
        game.run(options.numRuns == 1);
    }

    std::cout << "Game over.\n";
    game.printScores();
}
