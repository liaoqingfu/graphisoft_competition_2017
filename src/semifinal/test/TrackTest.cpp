#include "Track.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TrackTest)

BOOST_AUTO_TEST_SUITE(TrackTest)

BOOST_AUTO_TEST_SUITE(Construction)

BOOST_AUTO_TEST_CASE(Construct) {
    const Track::Monitors monitors{Point{0, 0}, Point{1, 0}};
    const Track::Princesses princesses{Point{1, 0}, Point{0, 1}};
    Track track{2, 2, {1, 2, 3, 4}, monitors, princesses};
    BOOST_TEST_MESSAGE(track);

    BOOST_TEST(track.getField({0, 0}).type == 1);
    BOOST_TEST(track.getField({0, 0}).monitor);
    BOOST_TEST(track.getField({0, 0}).princess == -1);
    BOOST_TEST(track.getField({1, 0}).type == 2);
    BOOST_TEST(track.getField({1, 0}).monitor);
    BOOST_TEST(track.getField({1, 0}).princess == 0);
    BOOST_TEST(track.getField({0, 1}).type == 3);
    BOOST_TEST(!track.getField({0, 1}).monitor);
    BOOST_TEST(track.getField({0, 1}).princess == 1);
    BOOST_TEST(track.getField({1, 1}).type == 4);
    BOOST_TEST(!track.getField({1, 1}).monitor);
    BOOST_TEST(track.getField({1, 1}).princess == -1);

    BOOST_TEST(track.getMonitor(0) == monitors[0]);
    BOOST_TEST(track.getMonitor(1) == monitors[1]);
    BOOST_TEST(track.getPrincess(0) == princesses[0]);
    BOOST_TEST(track.getPrincess(1) == princesses[1]);
}

BOOST_AUTO_TEST_SUITE_END() // Construction

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

BOOST_AUTO_TEST_SUITE_END() // TrackTest
