#include "GameState.hpp"
#include <exception>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

GameState parseInitialGameState(const std::vector<std::string>& input) {
    GameState result;
    using namespace boost;
    for (const auto& line : input) {
        std::vector<std::string> tokens;
        split(tokens, line, is_any_of(" "));

        if (tokens.size() == 0) {
            throw std::runtime_error("received empty line from server");
        }

        if (tokens.at(0) == "MESSAGE") {
            if (tokens.size() == 1) { // auth error
                throw std::runtime_error("auth error");
            }
            if (tokens.at(1) != "OK") {
                throw std::runtime_error(
                    std::string("auth error: ").append(tokens.at(1)));
            }
        } else if (tokens.at(0) == "LEVEL") {
            // TODO check level is the same we requested
            // auto level = std::stoi(tokens.at(1));
        } else if (tokens.at(0) == "SIZE") {
            result.height = std::stoi(tokens.at(1)); // N
            result.width = std::stoi(tokens.at(2));  // M
        } else if (tokens.at(0) == "DISPLAYS") {
            result.numDisplays = std::stoi(tokens.at(1));
        } else if (tokens.at(0) == "PLAYER") {
            result.playerId = std::stoi(tokens.at(1));
        } else if (tokens.at(0) == "MAXTICK") {
            result.maxTick = std::stoi(tokens.at(1));
        }
    }
    return result;
}

void parseTickInfo(GameState& gs, const std::vector<std::string>& input) {
}

std::vector<std::string> createOutput(const Step& step) { return {}; }

