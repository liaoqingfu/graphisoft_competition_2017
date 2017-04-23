#include "ChoosingStrategy.hpp"

#include <boost/test/unit_test.hpp>

struct OpponentsInfoTestFixture {
    OpponentsInfo& getOpponentsInfo(ChoosingStrategy& cs) {
        return cs.opponentsInfo;
    }
    void updateOpponentsInfo(ChoosingStrategy& cs, const Track& track,
                             int playerId) {
        return cs.updateOpponentsInfo(track, playerId);
    }
    ChoosingStrategy cs;
    const Track::Monitors monitors{};
    const Track::Princesses princesses{Point{0,0}};
    int playerId = 0;
};

BOOST_AUTO_TEST_SUITE(ChoosingStrategyTest)

BOOST_FIXTURE_TEST_CASE(ShiftRight, OpponentsInfoTestFixture) {
    Track prevTrack{3, 3, {1, 2, 3, 4, 5, 6, 7, 8, 9}, monitors, princesses};
    // ExtraField at {0,0}
    Track currentTrack{3, 3, {13, 1, 2, 4, 5, 6, 7, 8, 9}, monitors, princesses};
    updateOpponentsInfo(cs, prevTrack, playerId);
    updateOpponentsInfo(cs, currentTrack, playerId);
    auto oppInfo = getOpponentsInfo(cs);
    BOOST_CHECK_EQUAL(oppInfo[0].extraField, 3);
}

BOOST_FIXTURE_TEST_CASE(ShiftRightMiddle, OpponentsInfoTestFixture) {
    Track prevTrack{3, 3, {1, 2, 3, 4, 5, 6, 7, 8, 9}, monitors, princesses};
    // ExtraField at {0,0}
    Track currentTrack{3, 3, {1, 2, 3, 13, 4, 5, 7, 8, 9}, monitors, princesses};
    updateOpponentsInfo(cs, prevTrack, playerId);
    updateOpponentsInfo(cs, currentTrack, playerId);
    auto oppInfo = getOpponentsInfo(cs);
    BOOST_CHECK_EQUAL(oppInfo[0].extraField, 6);
}

BOOST_FIXTURE_TEST_CASE(ShiftLeft, OpponentsInfoTestFixture) {
    Track prevTrack{3, 3, {1, 2, 3, 4, 5, 6, 7, 8, 9}, monitors, princesses};
    // ExtraField at {0,0}
    Track currentTrack{3, 3, {2, 3, 13, 4, 5, 6, 7, 8, 9}, monitors, princesses};
    updateOpponentsInfo(cs, prevTrack, playerId);
    updateOpponentsInfo(cs, currentTrack, playerId);
    auto oppInfo = getOpponentsInfo(cs);
    BOOST_CHECK_EQUAL(oppInfo[0].extraField, 1);
}

BOOST_FIXTURE_TEST_CASE(ShiftUp, OpponentsInfoTestFixture) {
    Track prevTrack{3, 3, {1, 2, 3, 4, 5, 6, 7, 8, 9}, monitors, princesses};
    // ExtraField at {0,0}
    Track currentTrack{3, 3, {4, 2, 3, 7, 5, 6, 13, 8, 9}, monitors, princesses};
    updateOpponentsInfo(cs, prevTrack, playerId);
    updateOpponentsInfo(cs, currentTrack, playerId);
    auto oppInfo = getOpponentsInfo(cs);
    BOOST_CHECK_EQUAL(oppInfo[0].extraField, 1);
}

BOOST_FIXTURE_TEST_CASE(ShiftUpMiddle, OpponentsInfoTestFixture) {
    Track prevTrack{3, 3, {1, 2, 3, 4, 5, 6, 7, 8, 9}, monitors, princesses};
    // ExtraField at {0,0}
    Track currentTrack{3, 3, {1, 5, 3, 4, 8, 6, 7, 13, 9}, monitors, princesses};
    updateOpponentsInfo(cs, prevTrack, playerId);
    updateOpponentsInfo(cs, currentTrack, playerId);
    auto oppInfo = getOpponentsInfo(cs);
    BOOST_CHECK_EQUAL(oppInfo[0].extraField, 2);
}

BOOST_FIXTURE_TEST_CASE(ShiftDown, OpponentsInfoTestFixture) {
    Track prevTrack{3, 3, {1, 2, 3, 4, 5, 6, 7, 8, 9}, monitors, princesses};
    // ExtraField at {0,0}
    Track currentTrack{3, 3, {13, 2, 3, 1, 5, 6, 4, 8, 9}, monitors, princesses};
    updateOpponentsInfo(cs, prevTrack, playerId);
    updateOpponentsInfo(cs, currentTrack, playerId);
    auto oppInfo = getOpponentsInfo(cs);
    BOOST_CHECK_EQUAL(oppInfo[0].extraField, 7);
}

BOOST_AUTO_TEST_CASE(GetIsomorphs) {
}

BOOST_AUTO_TEST_SUITE_END() // ChoosingStrategyTest
