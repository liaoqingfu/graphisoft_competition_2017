#include "GameState.hpp"
#include <exception>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace {

std::vector<std::string> split(const std::string& line) {
    using namespace boost;
    std::vector<std::string> tokens;
    split(tokens, line, is_any_of(" "));
    return tokens;
}

} // namespace

using std::stoi;

GameState parseInitialGameState(const std::vector<std::string>& input) {
    GameState result;
    for (const auto& line : input) {
        auto tokens = split(line);

        if (tokens.size() == 0) {
            throw std::runtime_error("init: received empty line from server");
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
            // auto level = stoi(tokens.at(1));
        } else if (tokens.at(0) == "SIZE") {
            result.height = stoi(tokens.at(1)); // N
            result.width = stoi(tokens.at(2));  // M
        } else if (tokens.at(0) == "DISPLAYS") {
            result.numDisplays = stoi(tokens.at(1));
        } else if (tokens.at(0) == "PLAYER") {
            result.playerId = stoi(tokens.at(1));
        } else if (tokens.at(0) == "MAXTICK") {
            result.maxTick = stoi(tokens.at(1));
        }
    }
    return result;
}

void parseTickInfo(GameState& gs, const std::vector<std::string>& input) {

    std::vector<int> fields;
    Track::Monitors monitors(gs.numDisplays);
    Track::Princesses princesses(gs.numPlayers);
    for (const auto& line : input) {
        auto tokens = split(line);

        if (tokens.size() == 0) {
            throw std::runtime_error("tick: received empty line from server");
        }

        if (tokens.at(0) == "MESSAGE") {
            if (tokens.size() > 1 && tokens.at(1) != "OK") {
                // TODO instead of throw we might need this as a bool return
                // value
                throw std::runtime_error(
                    std::string("error in previous command: ")
                        .append(tokens.at(1)));
            }
        } else if (tokens.at(0) == "TICK") {
            gs.currentTick = stoi(tokens.at(1));
        } else if (tokens.at(0) == "FIELDS") {
            if ((int)tokens.size() - 1 != gs.width * gs.height) {
                throw std::runtime_error("incorrect number of fields");
            }
            for (std::size_t i = 1; i < tokens.size(); ++i) {
                fields.push_back(stoi(tokens[i]));
            }
        } else if (tokens.at(0) == "DISPLAY") {
            auto i = stoi(tokens.at(1));
            monitors.at(i) = Point{stoi(tokens.at(2)), stoi(tokens.at(3))};
        } else if (tokens.at(0) == "POSITION") {
            auto i = stoi(tokens.at(1));
            princesses.at(i) = Point{stoi(tokens.at(2)), stoi(tokens.at(3))};
        } else if (tokens.at(0) == "PLAYER") {
            gs.activePlayerId = stoi(tokens.at(1));
        } else if (tokens.at(0) == "TARGET") {
            auto i = stoi(tokens.at(1));
            if (i > gs.numDisplays -1) {
                throw std::runtime_error("incorrect target");
            }
            gs.targetMonitor = i;
        } else if (tokens.at(0) == "EXTRAFIELD") {
            gs.extraField = stoi(tokens.at(1));
        }
    } // for
    gs.track = Track(gs.width, gs.height, fields, monitors, princesses);
}

std::vector<std::string> createOutput(const Step& step) { return {}; }

