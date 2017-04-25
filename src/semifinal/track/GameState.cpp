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

GameInfo parseInitial(const std::vector<std::string>& input) {
    GameInfo result;
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
            result.width = stoi(tokens.at(1));  // N
            result.height = stoi(tokens.at(2)); // M
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
    const auto& gi = gs.gameInfo;
    Track::Monitors monitors(gi.numDisplays, Point{-1, -1});
    Track::Princesses princesses(gi.numPlayers);
    for (const auto& line : input) {
        auto tokens = split(line);

        if (tokens.size() == 0) {
            throw std::runtime_error("tick: received empty line from server");
        }

        if (tokens.at(0) == "MESSAGE") {
            if (tokens.size() > 1 && tokens.at(1) != "OK") {
                // TODO instead of throw we might need this as a bool return
                // value
                std::cerr << "Error received: " << line << std::endl;
                // throw std::runtime_error(
                //     std::string("error in previous command: ")
                //         .append(tokens.at(1)));
            }
        } else if (tokens.at(0) == "TICK") {
            gs.currentTick = stoi(tokens.at(1));
        } else if (tokens.at(0) == "FIELDS") {
            if ((int)tokens.size() - 1 != gi.width * gi.height) {
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
            if (i > gi.numDisplays -1) {
                throw std::runtime_error("incorrect target");
            }
            gs.targetMonitor = i;
        } else if (tokens.at(0) == "EXTRAFIELD") {
            gs.extraField = stoi(tokens.at(1));
        }
    } // for
    gs.track = Track(gi.width, gi.height, fields, monitors, princesses);
}

std::vector<std::string> createOutput(const Track& track,
        const Step& step, Point ourPosition) {
    Point newPosition = transformPoints(track, {ourPosition},
            step.pushDirection, step.pushPosition)[0].transformed;
    std::string c =
        (step.pushDirection == left || step.pushDirection == right) ? "0" : "1";
    std::string p =
        (step.pushDirection == right || step.pushDirection == down) ? "1" : "0";
    std::string k = std::to_string(step.pushPosition);
    std::string t = std::to_string(step.pushFieldType);
    auto pushMsg = std::string("PUSH ")
                       .append(c)
                       .append(" ")
                       .append(p)
                       .append(" ")
                       .append(k)
                       .append(" ")
                       .append(t);

    // skip GOTO at identity
    if (newPosition == step.princessTarget) {
        return {pushMsg};
    }

    auto gotoMsg = std::string("GOTO ")
                       .append(std::to_string(step.princessTarget.x))
                       .append(" ")
                       .append(std::to_string(step.princessTarget.y));

    return {pushMsg, gotoMsg};
}
