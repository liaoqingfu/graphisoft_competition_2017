#include "Transformations.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TransformationsTest)

BOOST_AUTO_TEST_SUITE(TransformPoints)

class Fixture {
public:
    Track track;

    Fixture() {
        std::vector<int> fields(16);
        std::generate(fields.begin(), fields.end(), []() { return 1; });
        track = Track{4, 4, fields, {}, {}};
    }
};

BOOST_FIXTURE_TEST_CASE(ShiftLeft, Fixture) {
    const std::vector<Point> points{Point{1, 2}, Point{1, 3}};
    auto transformedPoints = transformPoints(track, points, left, 2);

    BOOST_TEST_REQUIRE(transformedPoints.size() == points.size());
    BOOST_TEST(transformedPoints[0].original == points[0]);
    BOOST_CHECK_EQUAL(transformedPoints[0].transformed, (Point{0, 2}));
    BOOST_TEST(transformedPoints[1].original == points[1]);
    BOOST_TEST(transformedPoints[1].transformed == points[1]);
}

BOOST_FIXTURE_TEST_CASE(ShiftRight, Fixture) {
    const std::vector<Point> points{Point{1, 2}, Point{0, 1}};
    auto transformedPoints = transformPoints(track, points, right, 1);

    BOOST_TEST_REQUIRE(transformedPoints.size() == points.size());
    BOOST_TEST(transformedPoints[0].original == points[0]);
    BOOST_TEST(transformedPoints[0].transformed == points[0]);
    BOOST_TEST(transformedPoints[1].original == points[1]);
    BOOST_CHECK_EQUAL(transformedPoints[1].transformed, (Point{1, 1}));
}

BOOST_FIXTURE_TEST_CASE(ShiftUp, Fixture) {
    const std::vector<Point> points{Point{0, 0}, Point{2, 1}};
    auto transformedPoints = transformPoints(track, points, up, 2);

    BOOST_TEST_REQUIRE(transformedPoints.size() == points.size());
    BOOST_TEST(transformedPoints[0].original == points[0]);
    BOOST_TEST(transformedPoints[0].transformed == points[0]);
    BOOST_TEST(transformedPoints[1].original == points[1]);
    BOOST_CHECK_EQUAL(transformedPoints[1].transformed, (Point{2, 0}));
}

BOOST_FIXTURE_TEST_CASE(ShiftDown, Fixture) {
    const std::vector<Point> points{Point{1, 1}, Point{2, 1}};
    auto transformedPoints = transformPoints(track, points, down, 1);

    BOOST_TEST_REQUIRE(transformedPoints.size() == points.size());
    BOOST_TEST(transformedPoints[0].original == points[0]);
    BOOST_CHECK_EQUAL(transformedPoints[0].transformed, (Point{1, 2}));
    BOOST_TEST(transformedPoints[1].original == points[1]);
    BOOST_TEST(transformedPoints[1].transformed == points[1]);
}

BOOST_FIXTURE_TEST_CASE(RotateLeft, Fixture) {
    const std::vector<Point> points{Point{0, 1}};
    auto transformedPoints = transformPoints(track, points, left, 1);

    BOOST_TEST_REQUIRE(transformedPoints.size() == points.size());
    BOOST_TEST(transformedPoints[0].original == points[0]);
    BOOST_CHECK_EQUAL(transformedPoints[0].transformed, (Point{3, 1}));
}

BOOST_FIXTURE_TEST_CASE(RotateRight, Fixture) {
    const std::vector<Point> points{Point{3, 3}};
    auto transformedPoints = transformPoints(track, points, right, 3);

    BOOST_TEST_REQUIRE(transformedPoints.size() == points.size());
    BOOST_TEST(transformedPoints[0].original == points[0]);
    BOOST_CHECK_EQUAL(transformedPoints[0].transformed, (Point{0, 3}));
}

BOOST_FIXTURE_TEST_CASE(RotateUp, Fixture) {
    const std::vector<Point> points{Point{2, 0}};
    auto transformedPoints = transformPoints(track, points, up, 2);

    BOOST_TEST_REQUIRE(transformedPoints.size() == points.size());
    BOOST_TEST(transformedPoints[0].original == points[0]);
    BOOST_CHECK_EQUAL(transformedPoints[0].transformed, (Point{2, 3}));
}

BOOST_FIXTURE_TEST_CASE(RotateDown, Fixture) {
    const std::vector<Point> points{Point{2, 3}};
    auto transformedPoints = transformPoints(track, points, down, 2);

    BOOST_TEST_REQUIRE(transformedPoints.size() == points.size());
    BOOST_TEST(transformedPoints[0].original == points[0]);
    BOOST_CHECK_EQUAL(transformedPoints[0].transformed, (Point{2, 0}));
}

BOOST_FIXTURE_TEST_CASE(MultipleMoves, Fixture) {
    const std::vector<Point> points{Point{2, 1}, Point{2, 0}, Point{1, 2}};
    auto middleStep = transformPoints(track, points, down, 2);
    auto transformedPoints = transformPoints(track, middleStep, left, 2);

    BOOST_TEST_REQUIRE(transformedPoints.size() == points.size());
    BOOST_TEST(transformedPoints[0].original == points[0]);
    BOOST_CHECK_EQUAL(transformedPoints[0].transformed, (Point{1, 2}));
    BOOST_TEST(transformedPoints[1].original == points[1]);
    BOOST_CHECK_EQUAL(transformedPoints[1].transformed, (Point{2, 1}));
    BOOST_TEST(transformedPoints[2].original == points[2]);
    BOOST_CHECK_EQUAL(transformedPoints[2].transformed, (Point{0, 2}));
}

BOOST_AUTO_TEST_SUITE_END() // TransformPoints

BOOST_AUTO_TEST_SUITE_END() // TransformationsTest

