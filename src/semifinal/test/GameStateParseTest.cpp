#include "GameState.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(GameStateParse)

BOOST_AUTO_TEST_CASE(ParseInitialGameState) {
    std::vector<std::string> input = {
        "MESSAGE OK",
        "LEVEL 3",
        "SIZE 3 4",
        "DISPLAYS 2",
        "PLAYER 0",
        "MAXTICK 300"
    };
    auto gs = parseInitialGameState(input);
    BOOST_CHECK_EQUAL(gs.width, 4);
    BOOST_CHECK_EQUAL(gs.height, 3);
    BOOST_CHECK_EQUAL(gs.numDisplays, 2);
    BOOST_CHECK_EQUAL(gs.playerId, 0);
    BOOST_CHECK_EQUAL(gs.maxTick, 300);
}

BOOST_AUTO_TEST_CASE(ParseInitialGameState_shall_throw_on_wrong_number_format) {
    std::vector<std::string> input = {
        "MESSAGE OK",
        "LEVEL 3",
        "SIZE 3 4",
        "DISPLAYS asdfa_not_a_number",
        "PLAYER 0",
        "MAXTICK 300"
    };
    BOOST_CHECK_THROW(parseInitialGameState(input), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(ParseInitialGameState_shall_throw_on_MESSAGE) {
    std::vector<std::string> input = {
        "MESSAGE"
    };
    BOOST_CHECK_THROW(parseInitialGameState(input), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
