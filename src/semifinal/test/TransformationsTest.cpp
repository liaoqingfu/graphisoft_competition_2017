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
    std::vector<Point> points{Point{1, 2}, Point{1, 3}};
    transformPoints(track, points, left, 2);

    BOOST_CHECK_EQUAL(points[0], (Point{0, 2}));
    BOOST_CHECK_EQUAL(points[1], (Point{1, 3}));
}

BOOST_FIXTURE_TEST_CASE(ShiftRight, Fixture) {
    std::vector<Point> points{Point{1, 2}, Point{0, 1}};
    transformPoints(track, points, right, 1);

    BOOST_CHECK_EQUAL(points[0], (Point{1, 2}));
    BOOST_CHECK_EQUAL(points[1], (Point{1, 1}));
}

BOOST_FIXTURE_TEST_CASE(ShiftUp, Fixture) {
    std::vector<Point> points{Point{0, 0}, Point{2, 1}};
    transformPoints(track, points, up, 2);

    BOOST_CHECK_EQUAL(points[0], (Point{0, 0}));
    BOOST_CHECK_EQUAL(points[1], (Point{2, 0}));
}

BOOST_FIXTURE_TEST_CASE(ShiftDown, Fixture) {
    std::vector<Point> points{Point{1, 1}, Point{2, 1}};
    transformPoints(track, points, down, 1);

    BOOST_CHECK_EQUAL(points[0], (Point{1, 2}));
    BOOST_CHECK_EQUAL(points[1], (Point{2, 1}));
}

BOOST_FIXTURE_TEST_CASE(RotateLeft, Fixture) {
    std::vector<Point> points{Point{0, 1}};
    transformPoints(track, points, left, 1);

    BOOST_CHECK_EQUAL(points[0], (Point{3, 1}));
}

BOOST_FIXTURE_TEST_CASE(RotateRight, Fixture) {
    std::vector<Point> points{Point{3, 3}};
    transformPoints(track, points, right, 3);

    BOOST_CHECK_EQUAL(points[0], (Point{0, 3}));
}

BOOST_FIXTURE_TEST_CASE(RotateUp, Fixture) {
    std::vector<Point> points{Point{2, 0}};
    transformPoints(track, points, up, 2);

    BOOST_CHECK_EQUAL(points[0], (Point{2, 3}));
}

BOOST_FIXTURE_TEST_CASE(RotateDown, Fixture) {
    std::vector<Point> points{Point{2, 3}};
    transformPoints(track, points, down, 2);

    BOOST_CHECK_EQUAL(points[0], (Point{2, 0}));
}

BOOST_AUTO_TEST_SUITE_END() // TransformPoints

BOOST_AUTO_TEST_SUITE_END() // TransformationsTest

