#include "GameState.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(GameStateParse)

BOOST_AUTO_TEST_SUITE(ParseInitial)

BOOST_AUTO_TEST_CASE(OK) {
    std::vector<std::string> input = {
        "MESSAGE OK",
        "LEVEL 3",
        "SIZE 3 4",
        "DISPLAYS 2",
        "PLAYER 0",
        "MAXTICK 300"
    };
    auto gi = parseInitial(input);
    BOOST_CHECK_EQUAL(gi.width, 4);
    BOOST_CHECK_EQUAL(gi.height, 3);
    BOOST_CHECK_EQUAL(gi.numDisplays, 2);
    BOOST_CHECK_EQUAL(gi.playerId, 0);
    BOOST_CHECK_EQUAL(gi.maxTick, 300);
}

BOOST_AUTO_TEST_CASE(Shall_throw_on_wrong_number_format) {
    std::vector<std::string> input = {
        "MESSAGE OK",
        "LEVEL 3",
        "SIZE 3 4",
        "DISPLAYS asdfa_not_a_number",
        "PLAYER 0",
        "MAXTICK 300"
    };
    BOOST_CHECK_THROW(parseInitial(input), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(Shall_throw_on_MESSAGE) {
    std::vector<std::string> input = {
        "MESSAGE"
    };
    BOOST_CHECK_THROW(parseInitial(input), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END() // parseInitial

BOOST_AUTO_TEST_SUITE(ParseTickInfo)

BOOST_AUTO_TEST_CASE(OK) {
    std::vector<std::string> input = {
        "MESSAGE OK",
        "LEVEL 3",
        "SIZE 3 4",
        "DISPLAYS 4",
        "PLAYER 0",
        "MAXTICK 300"
    };
    auto gi = parseInitial(input);
    input = {
        "TICK 0",
        "FIELDS 5 5 5 5 5 5 2 10 12 12 10 11",
        "DISPLAY 0 1 1",
        "DISPLAY 1 2 2",
        "DISPLAY 2 2 1",
        "DISPLAY 3 1 2",
        "POSITION 0 0 0",
        "POSITION 1 0 1",
        "POSITION 2 2 0",
        "POSITION 3 2 2",
        "PLAYER 0",
        "MESSAGE OK",
        "TARGET 2",
        "EXTRAFIELD 7"
    };
    GameState gs{gi};
    parseTickInfo(gs, input);
    BOOST_CHECK_EQUAL(gs.currentTick, 0);
    BOOST_CHECK_EQUAL(gs.activePlayerId, 0);
    BOOST_CHECK_EQUAL(gs.targetMonitor, 2);
    BOOST_CHECK_EQUAL(gs.extraField, 7);

    BOOST_CHECK_EQUAL(gs.track.height(), 3);
    BOOST_CHECK_EQUAL(gs.track.width(), 4);
    BOOST_CHECK_EQUAL(gs.track.height(), gi.height);
    BOOST_CHECK_EQUAL(gs.track.width(), gi.width);

    BOOST_CHECK_EQUAL(gs.track.getField(Point(0,0)).type, 5);
    BOOST_CHECK_EQUAL(gs.track.getField(Point(3,2)).type, 11);

    BOOST_CHECK_EQUAL(gs.track.getMonitor(0), Point(1, 1));
    BOOST_CHECK_EQUAL(gs.track.getMonitor(1), Point(2, 2));
    BOOST_CHECK_EQUAL(gs.track.getMonitor(2), Point(2, 1));
    BOOST_CHECK_EQUAL(gs.track.getMonitor(3), Point(1, 2));
    BOOST_CHECK_EQUAL(gs.track.getPrincess(0), Point(0, 0));
    BOOST_CHECK_EQUAL(gs.track.getPrincess(1), Point(0, 1));
    BOOST_CHECK_EQUAL(gs.track.getPrincess(2), Point(2, 0));
    BOOST_CHECK_EQUAL(gs.track.getPrincess(3), Point(2, 2));
}

BOOST_AUTO_TEST_CASE(IncorrectFields) {
    std::vector<std::string> input = {
        "MESSAGE OK",
        "LEVEL 3",
        "SIZE 3 4",
        "DISPLAYS 4",
        "PLAYER 0",
        "MAXTICK 300"
    };
    auto gs = GameState(parseInitial(input));
    input = {
        "FIELDS 1 2 3 4 5 6 7 8 9 10 11", // one less
    };
    BOOST_CHECK_THROW(parseTickInfo(gs, input), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(IncorrectTarget) {
    std::vector<std::string> input = {
        "MESSAGE OK",
        "LEVEL 3",
        "SIZE 3 4",
        "DISPLAYS 4",
        "PLAYER 0",
        "MAXTICK 300"
    };
    auto gs = GameState(parseInitial(input));
    input = {
        "TARGET 5"
    };
    BOOST_CHECK_THROW(parseTickInfo(gs, input), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(MessageError) {
    std::vector<std::string> input = {
        "MESSAGE OK",
        "LEVEL 3",
        "SIZE 3 4",
        "DISPLAYS 4",
        "PLAYER 0",
        "MAXTICK 300"
    };
    auto gs = GameState(parseInitial(input));
    input = {
        "MESSAGE ErrorString"
    };
    BOOST_CHECK_THROW(parseTickInfo(gs, input), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END() // ParseTickInfo

BOOST_AUTO_TEST_SUITE(CreateOutput)

BOOST_AUTO_TEST_CASE(OK) {
    Step step = {left, 1, 3, {2,3}};
    auto output = createOutput(step);
    BOOST_CHECK_EQUAL(output.size(), 2);
    BOOST_CHECK_EQUAL(output.at(0), "PUSH 1 0 1 3");
    BOOST_CHECK_EQUAL(output.at(1), "GOTO 2 3");
}

BOOST_AUTO_TEST_SUITE_END() // CreateOutput

BOOST_AUTO_TEST_SUITE_END()
