#include "Track.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TrackTest)

BOOST_AUTO_TEST_SUITE(Construction)

BOOST_AUTO_TEST_CASE(Construct) {
    const Track::Monitors monitors{Point{0, 0}, Point{1, 0}};
    const Track::Princesses princesses{Point{1, 0}, Point{0, 1}};
    Track track{2, 2, {1, 2, 3, 4}, monitors, princesses};
    BOOST_TEST_MESSAGE(track);

    BOOST_TEST(track.getField({0, 0}).type == 1);
    BOOST_TEST(track.getField({0, 0}).monitor == 0);
    BOOST_TEST(track.getField({0, 0}).princess == -1);
    BOOST_TEST(track.getField({1, 0}).type == 2);
    BOOST_TEST(track.getField({1, 0}).monitor == 1);
    BOOST_TEST(track.getField({1, 0}).princess == 0);
    BOOST_TEST(track.getField({0, 1}).type == 3);
    BOOST_TEST(track.getField({0, 1}).monitor == -1);
    BOOST_TEST(track.getField({0, 1}).princess == 1);
    BOOST_TEST(track.getField({1, 1}).type == 4);
    BOOST_TEST(track.getField({1, 1}).monitor == -1);
    BOOST_TEST(track.getField({1, 1}).princess == -1);

    BOOST_TEST(track.getMonitor(0) == monitors[0]);
    BOOST_TEST(track.getMonitor(1) == monitors[1]);
    BOOST_TEST(track.getPrincess(0) == princesses[0]);
    BOOST_TEST(track.getPrincess(1) == princesses[1]);
}

BOOST_AUTO_TEST_SUITE_END() // Construction

BOOST_AUTO_TEST_SUITE(CanMovePrincess)

BOOST_AUTO_TEST_CASE(StraightLineHorizontal) {
    Track track{6, 1, {10, 10, 2, 8, 10, 10}, {},
            {Point{0, 0}, Point{5, 0}}};
    BOOST_TEST_MESSAGE(track);

    BOOST_TEST(track.canMovePrincess(0, Point{0, 0}));
    BOOST_TEST(track.canMovePrincess(0, Point{1, 0}));
    BOOST_TEST(track.canMovePrincess(0, Point{2, 0}));
    BOOST_TEST(!track.canMovePrincess(0, Point{3, 0}));
    BOOST_TEST(!track.canMovePrincess(0, Point{4, 0}));
    BOOST_TEST(!track.canMovePrincess(0, Point{5, 0}));

    BOOST_TEST(!track.canMovePrincess(1, Point{0, 0}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 0}));
    BOOST_TEST(!track.canMovePrincess(1, Point{2, 0}));
    BOOST_TEST(track.canMovePrincess(1, Point{3, 0}));
    BOOST_TEST(track.canMovePrincess(1, Point{4, 0}));
    BOOST_TEST(track.canMovePrincess(1, Point{5, 0}));
}

BOOST_AUTO_TEST_CASE(HalfFitHorizontal) {
    Track track{2, 1, {2, 2}, {},
            {Point{0, 0}, Point{1, 0}}};
    BOOST_TEST_MESSAGE(track);

    BOOST_TEST(!track.canMovePrincess(0, Point{1, 0}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 0}));
}

BOOST_AUTO_TEST_CASE(TCrossHorizontal) {
    Track track{6, 2, {
             8, 14,  2,  8, 11,  2,
             8, 11,  2,  8, 11,  2}, {},
            {Point{0, 0}, Point{5, 0}}};
    BOOST_TEST_MESSAGE(track);

    BOOST_TEST(track.canMovePrincess(0, Point{0, 0}));
    BOOST_TEST(track.canMovePrincess(0, Point{1, 0}));
    BOOST_TEST(track.canMovePrincess(0, Point{2, 0}));
    BOOST_TEST(!track.canMovePrincess(0, Point{3, 0}));
    BOOST_TEST(!track.canMovePrincess(0, Point{4, 0}));
    BOOST_TEST(!track.canMovePrincess(0, Point{5, 0}));
    BOOST_TEST(track.canMovePrincess(0, Point{0, 1}));
    BOOST_TEST(track.canMovePrincess(0, Point{1, 1}));
    BOOST_TEST(track.canMovePrincess(0, Point{2, 1}));
    BOOST_TEST(!track.canMovePrincess(0, Point{3, 1}));
    BOOST_TEST(!track.canMovePrincess(0, Point{4, 1}));
    BOOST_TEST(!track.canMovePrincess(0, Point{5, 1}));

    BOOST_TEST(!track.canMovePrincess(1, Point{0, 0}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 0}));
    BOOST_TEST(!track.canMovePrincess(1, Point{2, 0}));
    BOOST_TEST(track.canMovePrincess(1, Point{3, 0}));
    BOOST_TEST(track.canMovePrincess(1, Point{4, 0}));
    BOOST_TEST(track.canMovePrincess(1, Point{5, 0}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 1}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 1}));
    BOOST_TEST(!track.canMovePrincess(1, Point{2, 1}));
    BOOST_TEST(!track.canMovePrincess(1, Point{3, 1}));
    BOOST_TEST(!track.canMovePrincess(1, Point{4, 1}));
    BOOST_TEST(!track.canMovePrincess(1, Point{5, 1}));
}

BOOST_AUTO_TEST_CASE(StraightLineVertical) {
    Track track{1, 6, {5, 5, 1, 4, 5, 5}, {},
            {Point{0, 0}, Point{0, 5}}};
    BOOST_TEST_MESSAGE(track);

    BOOST_TEST(track.canMovePrincess(0, Point{0, 0}));
    BOOST_TEST(track.canMovePrincess(0, Point{0, 1}));
    BOOST_TEST(track.canMovePrincess(0, Point{0, 2}));
    BOOST_TEST(!track.canMovePrincess(0, Point{0, 3}));
    BOOST_TEST(!track.canMovePrincess(0, Point{0, 4}));
    BOOST_TEST(!track.canMovePrincess(0, Point{0, 5}));

    BOOST_TEST(!track.canMovePrincess(1, Point{0, 0}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 1}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 2}));
    BOOST_TEST(track.canMovePrincess(1, Point{0, 3}));
    BOOST_TEST(track.canMovePrincess(1, Point{0, 4}));
    BOOST_TEST(track.canMovePrincess(1, Point{0, 5}));
}

BOOST_AUTO_TEST_CASE(HalfFitVertical) {
    Track track{1, 2, {1, 1}, {},
            {Point{0, 0}, Point{0, 1}}};
    BOOST_TEST_MESSAGE(track);

    BOOST_TEST(!track.canMovePrincess(0, Point{0, 1}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 0}));
}

BOOST_AUTO_TEST_CASE(TCrossVertical) {
    Track track{2, 6, {
             4,  4,
            13,  7,
             1,  1,
             4,  4,
            13, 13,
             1,  1}, {},
            {Point{0, 0}, Point{0, 5}}};
    BOOST_TEST_MESSAGE(track);

    BOOST_TEST(track.canMovePrincess(0, Point{0, 0}));
    BOOST_TEST(track.canMovePrincess(0, Point{0, 1}));
    BOOST_TEST(track.canMovePrincess(0, Point{0, 2}));
    BOOST_TEST(!track.canMovePrincess(0, Point{0, 3}));
    BOOST_TEST(!track.canMovePrincess(0, Point{0, 4}));
    BOOST_TEST(!track.canMovePrincess(0, Point{0, 5}));
    BOOST_TEST(track.canMovePrincess(0, Point{1, 0}));
    BOOST_TEST(track.canMovePrincess(0, Point{1, 1}));
    BOOST_TEST(track.canMovePrincess(0, Point{1, 2}));
    BOOST_TEST(!track.canMovePrincess(0, Point{1, 3}));
    BOOST_TEST(!track.canMovePrincess(0, Point{1, 4}));
    BOOST_TEST(!track.canMovePrincess(0, Point{1, 5}));

    BOOST_TEST(!track.canMovePrincess(1, Point{0, 0}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 1}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 2}));
    BOOST_TEST(track.canMovePrincess(1, Point{0, 3}));
    BOOST_TEST(track.canMovePrincess(1, Point{0, 4}));
    BOOST_TEST(track.canMovePrincess(1, Point{0, 5}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 0}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 1}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 2}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 3}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 4}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 5}));
}

BOOST_AUTO_TEST_CASE(Curves) {
    Track track{4, 10, {
            10,  6, 12, 10,
            10,  5,  5, 10,
            12,  3,  9,  6,
             5, 10, 10,  5,
             9,  2,  8,  3,
            10,  6, 10,  6,
             5,  5,  5,  5,
            12,  3, 12,  3,
             5,  5,  5,  5,
             9,  2,  9,  2}, {},
            {Point{0, 0}, Point{3, 0}, Point{0, 9}, Point{3, 9}}};
    BOOST_TEST_MESSAGE(track);

    BOOST_TEST(track.canMovePrincess(0, Point{0, 0}));
    BOOST_TEST(track.canMovePrincess(0, Point{1, 0}));
    BOOST_TEST(!track.canMovePrincess(0, Point{2, 0}));
    BOOST_TEST(!track.canMovePrincess(0, Point{3, 0}));
    BOOST_TEST(!track.canMovePrincess(0, Point{0, 1}));
    BOOST_TEST(track.canMovePrincess(0, Point{1, 1}));
    BOOST_TEST(!track.canMovePrincess(0, Point{2, 1}));
    BOOST_TEST(!track.canMovePrincess(0, Point{3, 1}));
    BOOST_TEST(track.canMovePrincess(0, Point{0, 2}));
    BOOST_TEST(track.canMovePrincess(0, Point{1, 2}));
    BOOST_TEST(!track.canMovePrincess(0, Point{2, 2}));
    BOOST_TEST(!track.canMovePrincess(0, Point{3, 2}));
    BOOST_TEST(track.canMovePrincess(0, Point{0, 3}));
    BOOST_TEST(!track.canMovePrincess(0, Point{1, 3}));
    BOOST_TEST(!track.canMovePrincess(0, Point{2, 3}));
    BOOST_TEST(!track.canMovePrincess(0, Point{3, 3}));
    BOOST_TEST(track.canMovePrincess(0, Point{0, 4}));
    BOOST_TEST(track.canMovePrincess(0, Point{1, 4}));
    BOOST_TEST(!track.canMovePrincess(0, Point{2, 4}));
    BOOST_TEST(!track.canMovePrincess(0, Point{3, 4}));
    BOOST_TEST(!track.canMovePrincess(0, Point{0, 5}));
    BOOST_TEST(!track.canMovePrincess(0, Point{1, 5}));
    BOOST_TEST(!track.canMovePrincess(0, Point{2, 5}));
    BOOST_TEST(!track.canMovePrincess(0, Point{3, 5}));
    BOOST_TEST(!track.canMovePrincess(0, Point{0, 6}));
    BOOST_TEST(!track.canMovePrincess(0, Point{1, 6}));
    BOOST_TEST(!track.canMovePrincess(0, Point{2, 6}));
    BOOST_TEST(!track.canMovePrincess(0, Point{3, 6}));
    BOOST_TEST(!track.canMovePrincess(0, Point{0, 7}));
    BOOST_TEST(!track.canMovePrincess(0, Point{1, 7}));
    BOOST_TEST(!track.canMovePrincess(0, Point{2, 7}));
    BOOST_TEST(!track.canMovePrincess(0, Point{3, 7}));
    BOOST_TEST(!track.canMovePrincess(0, Point{0, 8}));
    BOOST_TEST(!track.canMovePrincess(0, Point{1, 8}));
    BOOST_TEST(!track.canMovePrincess(0, Point{2, 8}));
    BOOST_TEST(!track.canMovePrincess(0, Point{3, 8}));
    BOOST_TEST(!track.canMovePrincess(0, Point{0, 9}));
    BOOST_TEST(!track.canMovePrincess(0, Point{1, 9}));
    BOOST_TEST(!track.canMovePrincess(0, Point{2, 9}));
    BOOST_TEST(!track.canMovePrincess(0, Point{3, 9}));

    BOOST_TEST(!track.canMovePrincess(1, Point{0, 0}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 0}));
    BOOST_TEST(track.canMovePrincess(1, Point{2, 0}));
    BOOST_TEST(track.canMovePrincess(1, Point{3, 0}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 1}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 1}));
    BOOST_TEST(track.canMovePrincess(1, Point{2, 1}));
    BOOST_TEST(!track.canMovePrincess(1, Point{3, 1}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 2}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 2}));
    BOOST_TEST(track.canMovePrincess(1, Point{2, 2}));
    BOOST_TEST(track.canMovePrincess(1, Point{3, 2}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 3}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 3}));
    BOOST_TEST(!track.canMovePrincess(1, Point{2, 3}));
    BOOST_TEST(track.canMovePrincess(1, Point{3, 3}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 4}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 4}));
    BOOST_TEST(track.canMovePrincess(1, Point{2, 4}));
    BOOST_TEST(track.canMovePrincess(1, Point{3, 4}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 5}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 5}));
    BOOST_TEST(!track.canMovePrincess(1, Point{2, 5}));
    BOOST_TEST(!track.canMovePrincess(1, Point{3, 5}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 6}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 6}));
    BOOST_TEST(!track.canMovePrincess(1, Point{2, 6}));
    BOOST_TEST(!track.canMovePrincess(1, Point{3, 6}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 7}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 7}));
    BOOST_TEST(!track.canMovePrincess(1, Point{2, 7}));
    BOOST_TEST(!track.canMovePrincess(1, Point{3, 7}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 8}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 8}));
    BOOST_TEST(!track.canMovePrincess(1, Point{2, 8}));
    BOOST_TEST(!track.canMovePrincess(1, Point{3, 8}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 9}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 9}));
    BOOST_TEST(!track.canMovePrincess(1, Point{2, 9}));
    BOOST_TEST(!track.canMovePrincess(1, Point{3, 9}));

    BOOST_TEST(!track.canMovePrincess(2, Point{0, 0}));
    BOOST_TEST(!track.canMovePrincess(2, Point{1, 0}));
    BOOST_TEST(!track.canMovePrincess(2, Point{2, 0}));
    BOOST_TEST(!track.canMovePrincess(2, Point{3, 0}));
    BOOST_TEST(!track.canMovePrincess(2, Point{0, 1}));
    BOOST_TEST(!track.canMovePrincess(2, Point{1, 1}));
    BOOST_TEST(!track.canMovePrincess(2, Point{2, 1}));
    BOOST_TEST(!track.canMovePrincess(2, Point{3, 1}));
    BOOST_TEST(!track.canMovePrincess(2, Point{0, 2}));
    BOOST_TEST(!track.canMovePrincess(2, Point{1, 2}));
    BOOST_TEST(!track.canMovePrincess(2, Point{2, 2}));
    BOOST_TEST(!track.canMovePrincess(2, Point{3, 2}));
    BOOST_TEST(!track.canMovePrincess(2, Point{0, 3}));
    BOOST_TEST(!track.canMovePrincess(2, Point{1, 3}));
    BOOST_TEST(!track.canMovePrincess(2, Point{2, 3}));
    BOOST_TEST(!track.canMovePrincess(2, Point{3, 3}));
    BOOST_TEST(!track.canMovePrincess(2, Point{0, 4}));
    BOOST_TEST(!track.canMovePrincess(2, Point{1, 4}));
    BOOST_TEST(!track.canMovePrincess(2, Point{2, 4}));
    BOOST_TEST(!track.canMovePrincess(2, Point{3, 4}));
    BOOST_TEST(track.canMovePrincess(2, Point{0, 5}));
    BOOST_TEST(track.canMovePrincess(2, Point{1, 5}));
    BOOST_TEST(!track.canMovePrincess(2, Point{2, 5}));
    BOOST_TEST(!track.canMovePrincess(2, Point{3, 5}));
    BOOST_TEST(!track.canMovePrincess(2, Point{0, 6}));
    BOOST_TEST(track.canMovePrincess(2, Point{1, 6}));
    BOOST_TEST(!track.canMovePrincess(2, Point{2, 6}));
    BOOST_TEST(!track.canMovePrincess(2, Point{3, 6}));
    BOOST_TEST(track.canMovePrincess(2, Point{0, 7}));
    BOOST_TEST(track.canMovePrincess(2, Point{1, 7}));
    BOOST_TEST(!track.canMovePrincess(2, Point{2, 7}));
    BOOST_TEST(!track.canMovePrincess(2, Point{3, 7}));
    BOOST_TEST(track.canMovePrincess(2, Point{0, 8}));
    BOOST_TEST(!track.canMovePrincess(2, Point{1, 8}));
    BOOST_TEST(!track.canMovePrincess(2, Point{2, 8}));
    BOOST_TEST(!track.canMovePrincess(2, Point{3, 8}));
    BOOST_TEST(track.canMovePrincess(2, Point{0, 9}));
    BOOST_TEST(track.canMovePrincess(2, Point{1, 9}));
    BOOST_TEST(!track.canMovePrincess(2, Point{2, 9}));
    BOOST_TEST(!track.canMovePrincess(2, Point{3, 9}));

    BOOST_TEST(!track.canMovePrincess(3, Point{0, 0}));
    BOOST_TEST(!track.canMovePrincess(3, Point{1, 0}));
    BOOST_TEST(!track.canMovePrincess(3, Point{2, 0}));
    BOOST_TEST(!track.canMovePrincess(3, Point{3, 0}));
    BOOST_TEST(!track.canMovePrincess(3, Point{0, 1}));
    BOOST_TEST(!track.canMovePrincess(3, Point{1, 1}));
    BOOST_TEST(!track.canMovePrincess(3, Point{2, 1}));
    BOOST_TEST(!track.canMovePrincess(3, Point{3, 1}));
    BOOST_TEST(!track.canMovePrincess(3, Point{0, 2}));
    BOOST_TEST(!track.canMovePrincess(3, Point{1, 2}));
    BOOST_TEST(!track.canMovePrincess(3, Point{2, 2}));
    BOOST_TEST(!track.canMovePrincess(3, Point{3, 2}));
    BOOST_TEST(!track.canMovePrincess(3, Point{0, 3}));
    BOOST_TEST(!track.canMovePrincess(3, Point{1, 3}));
    BOOST_TEST(!track.canMovePrincess(3, Point{2, 3}));
    BOOST_TEST(!track.canMovePrincess(3, Point{3, 3}));
    BOOST_TEST(!track.canMovePrincess(3, Point{0, 4}));
    BOOST_TEST(!track.canMovePrincess(3, Point{1, 4}));
    BOOST_TEST(!track.canMovePrincess(3, Point{2, 4}));
    BOOST_TEST(!track.canMovePrincess(3, Point{3, 4}));
    BOOST_TEST(!track.canMovePrincess(3, Point{0, 5}));
    BOOST_TEST(!track.canMovePrincess(3, Point{1, 5}));
    BOOST_TEST(track.canMovePrincess(3, Point{2, 5}));
    BOOST_TEST(track.canMovePrincess(3, Point{3, 5}));
    BOOST_TEST(!track.canMovePrincess(3, Point{0, 6}));
    BOOST_TEST(!track.canMovePrincess(3, Point{1, 6}));
    BOOST_TEST(!track.canMovePrincess(3, Point{2, 6}));
    BOOST_TEST(track.canMovePrincess(3, Point{3, 6}));
    BOOST_TEST(!track.canMovePrincess(3, Point{0, 7}));
    BOOST_TEST(!track.canMovePrincess(3, Point{1, 7}));
    BOOST_TEST(track.canMovePrincess(3, Point{2, 7}));
    BOOST_TEST(track.canMovePrincess(3, Point{3, 7}));
    BOOST_TEST(!track.canMovePrincess(3, Point{0, 8}));
    BOOST_TEST(!track.canMovePrincess(3, Point{1, 8}));
    BOOST_TEST(track.canMovePrincess(3, Point{2, 8}));
    BOOST_TEST(!track.canMovePrincess(3, Point{3, 8}));
    BOOST_TEST(!track.canMovePrincess(3, Point{0, 9}));
    BOOST_TEST(!track.canMovePrincess(3, Point{1, 9}));
    BOOST_TEST(track.canMovePrincess(3, Point{2, 9}));
    BOOST_TEST(track.canMovePrincess(3, Point{3, 9}));
}

BOOST_AUTO_TEST_CASE(Crosses) {
    Track track{4, 4, {
            12, 10, 10,  6,
             5, 15, 15,  5,
             5, 15, 15,  5,
             9, 10, 10,  3}, {},
            {Point{0, 0}, Point{1, 1}}};
    BOOST_TEST_MESSAGE(track);

    BOOST_TEST(track.canMovePrincess(0, Point{0, 0}));
    BOOST_TEST(track.canMovePrincess(0, Point{1, 0}));
    BOOST_TEST(track.canMovePrincess(0, Point{2, 0}));
    BOOST_TEST(track.canMovePrincess(0, Point{3, 0}));
    BOOST_TEST(track.canMovePrincess(0, Point{0, 1}));
    BOOST_TEST(!track.canMovePrincess(0, Point{1, 1}));
    BOOST_TEST(!track.canMovePrincess(0, Point{2, 1}));
    BOOST_TEST(track.canMovePrincess(0, Point{3, 1}));
    BOOST_TEST(track.canMovePrincess(0, Point{0, 2}));
    BOOST_TEST(!track.canMovePrincess(0, Point{1, 2}));
    BOOST_TEST(!track.canMovePrincess(0, Point{2, 2}));
    BOOST_TEST(track.canMovePrincess(0, Point{3, 2}));
    BOOST_TEST(track.canMovePrincess(0, Point{0, 3}));
    BOOST_TEST(track.canMovePrincess(0, Point{1, 3}));
    BOOST_TEST(track.canMovePrincess(0, Point{2, 3}));
    BOOST_TEST(track.canMovePrincess(0, Point{3, 3}));

    BOOST_TEST(!track.canMovePrincess(1, Point{0, 0}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 0}));
    BOOST_TEST(!track.canMovePrincess(1, Point{2, 0}));
    BOOST_TEST(!track.canMovePrincess(1, Point{3, 0}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 1}));
    BOOST_TEST(track.canMovePrincess(1, Point{1, 1}));
    BOOST_TEST(track.canMovePrincess(1, Point{2, 1}));
    BOOST_TEST(!track.canMovePrincess(1, Point{3, 1}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 2}));
    BOOST_TEST(track.canMovePrincess(1, Point{1, 2}));
    BOOST_TEST(track.canMovePrincess(1, Point{2, 2}));
    BOOST_TEST(!track.canMovePrincess(1, Point{3, 2}));
    BOOST_TEST(!track.canMovePrincess(1, Point{0, 3}));
    BOOST_TEST(!track.canMovePrincess(1, Point{1, 3}));
    BOOST_TEST(!track.canMovePrincess(1, Point{2, 3}));
    BOOST_TEST(!track.canMovePrincess(1, Point{3, 3}));
}

BOOST_AUTO_TEST_SUITE_END() // CanMovePrincess

BOOST_AUTO_TEST_SUITE(MoveFields)

BOOST_AUTO_TEST_CASE(MoveRight) {
    Track track{4, 4, {
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,
            13, 14, 15,  1}, {}, {}};
    BOOST_TEST_MESSAGE(track);

    track.moveFields(right, 1, 10);
    BOOST_TEST_MESSAGE(track);

    Track expectedResult{4, 4, {
             1,  2,  3,  4,
            10,  5,  6,  7,
             9, 10, 11, 12,
            13, 14, 15,  1}, {}, {}};
    BOOST_TEST(track == expectedResult);
}

BOOST_AUTO_TEST_CASE(MoveLeft) {
    Track track{4, 4, {
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,
            13, 14, 15,  1}, {}, {}};
    BOOST_TEST_MESSAGE(track);

    track.moveFields(left, 0, 6);
    BOOST_TEST_MESSAGE(track);

    Track expectedResult{4, 4, {
             2,  3,  4,  6,
             5,  6,  7,  8,
             9, 10, 11, 12,
            13, 14, 15,  1}, {}, {}};
    BOOST_TEST(track == expectedResult);
}

BOOST_AUTO_TEST_CASE(MoveDown) {
    Track track{4, 4, {
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,
            13, 14, 15,  1}, {}, {}};
    BOOST_TEST_MESSAGE(track);

    track.moveFields(down, 2, 8);
    BOOST_TEST_MESSAGE(track);

    Track expectedResult{4, 4, {
             1,  2,  8,  4,
             5,  6,  3,  8,
             9, 10,  7, 12,
            13, 14, 11,  1}, {}, {}};
    BOOST_TEST(track == expectedResult);
}

BOOST_AUTO_TEST_CASE(MoveUp) {
    Track track{4, 4, {
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,
            13, 14, 15,  1}, {}, {}};
    BOOST_TEST_MESSAGE(track);

    track.moveFields(up, 3, 15);
    BOOST_TEST_MESSAGE(track);

    Track expectedResult{4, 4, {
             1,  2,  3,  8,
             5,  6,  7, 12,
             9, 10, 11,  1,
            13, 14, 15, 15}, {}, {}};
    BOOST_TEST(track == expectedResult);
}

BOOST_AUTO_TEST_CASE(MonitorsMoved) {
    Track track{4, 4, {
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,
            13, 14, 15,  1},
            {Point{1, 0}, Point{1, 1}}, {}};
    BOOST_TEST_MESSAGE(track);

    track.moveFields(right, 1, 5);
    BOOST_TEST_MESSAGE(track);

    const Track::Monitors expectedMonitors{Point{1, 0}, Point{2, 1}};
    Track expectedResult{4, 4, {
             1,  2,  3,  4,
             5,  5,  6,  7,
             9, 10, 11, 12,
            13, 14, 15,  1}, expectedMonitors, {}};
    BOOST_TEST(track == expectedResult);
    BOOST_TEST(track.getMonitor(0) == expectedMonitors[0]);
    BOOST_TEST(track.getMonitor(1) == expectedMonitors[1]);
}

BOOST_AUTO_TEST_CASE(MonitorsRotated1) {
    Track track{3, 2, {
             1,  2,  3,
             4,  5,  6}, {Point{2, 0}}, {}};
    BOOST_TEST_MESSAGE(track);

    track.moveFields(right, 0, 10);
    BOOST_TEST_MESSAGE(track);

    const Track::Monitors expectedMonitors{Point{0, 0}};
    Track expectedResult{3, 2, {
            10,  1,  2,
             4,  5,  6}, expectedMonitors, {}};
    BOOST_TEST(track == expectedResult);
    BOOST_TEST(track.getMonitor(0) == expectedMonitors[0]);
}

BOOST_AUTO_TEST_CASE(MonitorsRotated2) {
    Track track{2, 3, {
             1,  2,
             3,  4,
             5,  6}, {Point{0, 0}}, {}};
    BOOST_TEST_MESSAGE(track);

    track.moveFields(up, 0, 11);
    BOOST_TEST_MESSAGE(track);

    const Track::Monitors expectedMonitors{Point{0, 2}};
    Track expectedResult{2, 3, {
             3,  2,
             5,  4,
            11,  6}, expectedMonitors, {}};
    BOOST_TEST(track == expectedResult);
    BOOST_TEST(track.getMonitor(0) == expectedMonitors[0]);
}

BOOST_AUTO_TEST_CASE(PrincessesMoved) {
    Track track{4, 4, {
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,
            13, 14, 15,  1}, {},
            {Point{0, 0}, Point{1, 1}, Point{1, 2}}};
    BOOST_TEST_MESSAGE(track);

    track.moveFields(up, 1, 5);
    BOOST_TEST_MESSAGE(track);

    const Track::Monitors expectedPrincesses{
            Point{0, 0}, Point{1, 0}, Point{1, 1}};
    Track expectedResult{4, 4, {
             1,  6,  3,  4,
             5, 10,  7,  8,
             9, 14, 11, 12,
            13,  5, 15,  1}, {}, expectedPrincesses};
    BOOST_TEST(track == expectedResult);
    BOOST_TEST(track.getPrincess(0) == expectedPrincesses[0]);
    BOOST_TEST(track.getPrincess(1) == expectedPrincesses[1]);
}

BOOST_AUTO_TEST_CASE(PrincessesRotated1) {
    Track track{3, 2, {
             1,  2,  3,
             4,  5,  6}, {}, {Point{0, 1}}};
    BOOST_TEST_MESSAGE(track);

    track.moveFields(left, 1, 10);
    BOOST_TEST_MESSAGE(track);

    const Track::Princesses expectedPrincesses{Point{2, 1}};
    Track expectedResult{3, 2, {
             1,  2,  3,
             5,  6, 10}, {}, expectedPrincesses};
    BOOST_TEST(track == expectedResult);
    BOOST_TEST(track.getPrincess(0) == expectedPrincesses[0]);
}

BOOST_AUTO_TEST_CASE(ReachabilityIsRecalculated) {
    Track track{4, 2, {
            10, 10, 14, 10,
             5,  5,  5,  5}, {}, {Point{0, 0}}};
    BOOST_TEST_MESSAGE(track);

    BOOST_TEST(track.canMovePrincess(0, Point{2, 1}));
    BOOST_TEST(!track.canMovePrincess(0, Point{3, 1}));

    track.moveFields(right, 0, 10);
    BOOST_TEST_MESSAGE(track);

    BOOST_TEST(!track.canMovePrincess(0, Point{2, 1}));
    BOOST_TEST(track.canMovePrincess(0, Point{3, 1}));
}

BOOST_AUTO_TEST_SUITE_END() // MoveFields

BOOST_AUTO_TEST_SUITE_END() // TrackTest
