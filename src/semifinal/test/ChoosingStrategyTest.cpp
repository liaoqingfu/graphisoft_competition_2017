#include "ChoosingStrategy.hpp"
#include "Track.hpp"

#include <boost/test/unit_test.hpp>

struct OpponentsInfoTestFixture {
    OpponentsInfo& getOpponentsInfo(ChoosingStrategy& cs) {
        return cs.opponentsInfo;
    }
    void updateOpponentsInfo(const Track& track, int playerId) {
        return cs.updateOpponentsInfo(track, playerId);
    }
    ChoosingStrategy cs;
    const Track::Monitors monitors{};
    const Track::Princesses princesses{Point{0,0}};
    int playerId = 0;
    Track prevTrack{3, 3, {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9}, monitors, princesses};
};

BOOST_AUTO_TEST_SUITE(ChoosingStrategyTest)

BOOST_FIXTURE_TEST_CASE(ShiftRight, OpponentsInfoTestFixture) {
    Track currentTrack = prevTrack;
    currentTrack.moveFields(right, 0, 13);
    updateOpponentsInfo(prevTrack, playerId);
    updateOpponentsInfo(currentTrack, playerId);
    auto oppInfo = getOpponentsInfo(cs);
    BOOST_CHECK_EQUAL(oppInfo[0].extraField, 3);
    BOOST_CHECK_EQUAL(oppInfo[0].lastPush, (Push{right, 0}));
}

BOOST_FIXTURE_TEST_CASE(ShiftRightMiddle, OpponentsInfoTestFixture) {
    Track prevTrack{3, 3, {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9}, monitors, princesses};
    Track currentTrack = prevTrack;
    currentTrack.moveFields(right, 1, 13);
    updateOpponentsInfo(prevTrack, playerId);
    updateOpponentsInfo(currentTrack, playerId);
    auto oppInfo = getOpponentsInfo(cs);
    BOOST_CHECK_EQUAL(oppInfo[0].extraField, 6);
    BOOST_CHECK_EQUAL(oppInfo[0].lastPush, (Push{right, 1}));
}

BOOST_FIXTURE_TEST_CASE(ShiftLeft, OpponentsInfoTestFixture) {
    Track currentTrack = prevTrack;
    currentTrack.moveFields(left, 0, 13);
    updateOpponentsInfo(prevTrack, playerId);
    updateOpponentsInfo(currentTrack, playerId);
    auto oppInfo = getOpponentsInfo(cs);
    BOOST_CHECK_EQUAL(oppInfo[0].extraField, 1);
    BOOST_CHECK_EQUAL(oppInfo[0].lastPush, (Push{left, 0}));
}

BOOST_FIXTURE_TEST_CASE(ShiftUp, OpponentsInfoTestFixture) {
    Track currentTrack = prevTrack;
    currentTrack.moveFields(up, 0, 13);
    updateOpponentsInfo(prevTrack, playerId);
    updateOpponentsInfo(currentTrack, playerId);
    auto oppInfo = getOpponentsInfo(cs);
    BOOST_CHECK_EQUAL(oppInfo[0].extraField, 1);
    BOOST_CHECK_EQUAL(oppInfo[0].lastPush, (Push{up, 0}));
}

BOOST_FIXTURE_TEST_CASE(ShiftUpMiddle, OpponentsInfoTestFixture) {
    Track currentTrack = prevTrack;
    currentTrack.moveFields(up, 1, 13);
    updateOpponentsInfo(prevTrack, playerId);
    updateOpponentsInfo(currentTrack, playerId);
    auto oppInfo = getOpponentsInfo(cs);
    BOOST_CHECK_EQUAL(oppInfo[0].extraField, 2);
    BOOST_CHECK_EQUAL(oppInfo[0].lastPush, (Push{up, 1}));
}

BOOST_FIXTURE_TEST_CASE(ShiftDown, OpponentsInfoTestFixture) {
    Track currentTrack = prevTrack;
    currentTrack.moveFields(down, 0, 13);
    updateOpponentsInfo(prevTrack, playerId);
    updateOpponentsInfo(currentTrack, playerId);
    auto oppInfo = getOpponentsInfo(cs);
    BOOST_CHECK_EQUAL(oppInfo[0].extraField, 7);
    BOOST_CHECK_EQUAL(oppInfo[0].lastPush, (Push{down, 0}));
}

BOOST_AUTO_TEST_CASE(GetIsomorphs) {
}

BOOST_AUTO_TEST_SUITE_END() // ChoosingStrategyTest
