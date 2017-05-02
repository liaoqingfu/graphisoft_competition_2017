#include "PotentialStep.hpp"

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <tuple>

BOOST_AUTO_TEST_SUITE(PotentialStepTest)

BOOST_AUTO_TEST_CASE(CalcualtePotentialSteps) {
    GameInfo gameInfo{3, 2, 0, 1, 1, 0};
    Point princessPosition{1, 0};
    Track originalTrack(3, 2, {
            1, 2, 3,
            4, 5, 6}, {}, {princessPosition});
    GameState gameState{gameInfo};
    gameState.track = originalTrack;
    OpponentsInfo opponentsInfo;

    auto steps = calculatePotentialSteps(gameState, opponentsInfo,
            0, 5);
    std::vector<Step> result;
    std::transform(steps.begin(), steps.end(), std::back_inserter(result),
            [](const PotentialStep& step) {
                return step.getStep();
            });
    std::sort(result.begin(), result.end(),
            [](const Step& lhs, const Step& rhs) {
                return std::make_tuple(
                        lhs.pushDirection, lhs.pushPosition,
                                lhs.pushFieldType, lhs.princessTarget)
                        < std::make_tuple(rhs.pushDirection, rhs.pushPosition,
                                rhs.pushFieldType, rhs.princessTarget);
            });

    std::vector<Step> expectedResult{
        {left, 0, 5, Point{0, 0}},
        {left, 0, 10, Point{0, 0}},
        {left, 1, 5, Point{1, 0}},
        {left, 1, 10, Point{1, 0}},
        {up, 0, 5, Point{1, 0}},
        {up, 0, 10, Point{1, 0}},
        {up, 1, 5, Point{1, 1}},
        {up, 1, 10, Point{1, 1}},
        {up, 2, 5, Point{1, 0}},
        {up, 2, 10, Point{1, 0}},
        {right, 0, 5, Point{2, 0}},
        {right, 0, 10, Point{2, 0}},
        {right, 1, 5, Point{1, 0}},
        {right, 1, 10, Point{1, 0}},
        {down, 0, 5, Point{1, 0}},
        {down, 0, 10, Point{1, 0}},
        {down, 1, 5, Point{1, 1}},
        {down, 1, 10, Point{1, 1}},
        {down, 2, 5, Point{1, 0}},
        {down, 2, 10, Point{1, 0}},
    };
    BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(),
            expectedResult.begin(), expectedResult.end());
}

BOOST_AUTO_TEST_SUITE_END()
