#include "GameState.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(GameStateTest)

BOOST_AUTO_TEST_CASE(TemporaryStepTest) {
    GameInfo gameInfo{3, 3, 0, 1, 1, 0, {}, {}};
    Track originalTrack(3, 3, {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9}, {}, {Point{1, 1}});
    Track expectedTrack(3, 3, {
            1, 2, 3,
            10, 4, 5,
            7, 8, 9}, {}, {Point{2, 2}});
    GameState gameState{gameInfo};
    gameState.track = originalTrack;

    auto temporaryStep = std::make_unique<TemporaryStep>(gameState,
            Step{right, 1, 10, Point{2, 2}});
    BOOST_TEST(gameState.track == expectedTrack);
    temporaryStep.reset();
    BOOST_TEST(gameState.track == originalTrack);
}

BOOST_AUTO_TEST_CASE(MovePoints) {
    GameInfo gameInfo{3, 3, 0, 1, 1, 0, {}, {}};
    Track originalTrack(3, 3, {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9}, {}, {Point{1, 1}});
    GameState gameState{gameInfo};
    gameState.track = originalTrack;
    std::vector<Point> ps{Point{0, 0}, Point{2, 1}};

    auto temporaryStep = std::make_unique<TemporaryStep>(gameState,
            Step{up, 2, 10, Point{1, 1}}, ps);
    BOOST_CHECK_EQUAL(ps[0], (Point{0, 0}));
    BOOST_CHECK_EQUAL(ps[1], (Point{2, 0}));
    temporaryStep.reset();
    BOOST_CHECK_EQUAL(ps[0], (Point{0, 0}));
    BOOST_CHECK_EQUAL(ps[1], (Point{2, 1}));
}

BOOST_AUTO_TEST_SUITE_END() // GameStateTest
