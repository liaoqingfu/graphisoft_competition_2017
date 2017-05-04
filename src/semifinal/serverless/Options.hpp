#ifndef SEMIFINAL_SERVERLESS_OPTIONS_HPP
#define SEMIFINAL_SERVERLESS_OPTIONS_HPP

#include <string>
#include <vector>

struct Options {
    int numRuns = 1;
    unsigned seed = 0;
    std::vector<std::string> strategyStrings;
    int width = 10;
    int height = 10;
    int numDisplays = 20;
    int maxTick = 40;
    std::size_t jobs = 1;
    bool debug;
    int numPlayers = 4;
};

Options parseOptions(int argc, const char* argv[]);

#endif // SEMIFINAL_SERVERLESS_OPTIONS_HPP
