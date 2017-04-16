#include "Field.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(FieldTest)

BOOST_AUTO_TEST_CASE(GetIsomorphs) {
    using V = std::vector<int>;
    BOOST_TEST((getIsomorphs(1) == V{1, 2, 4, 8}));
    BOOST_TEST((getIsomorphs(2) == V{1, 2, 4, 8}));
    BOOST_TEST((getIsomorphs(3) == V{3, 6, 9, 12}));
    BOOST_TEST((getIsomorphs(4) == V{1, 2, 4, 8}));
    BOOST_TEST((getIsomorphs(5) == V{5, 10}));
    BOOST_TEST((getIsomorphs(6) == V{3, 6, 9, 12}));
    BOOST_TEST((getIsomorphs(7) == V{7, 11, 13, 14}));
    BOOST_TEST((getIsomorphs(8) == V{1, 2, 4, 8}));
    BOOST_TEST((getIsomorphs(9) == V{3, 6, 9, 12}));
    BOOST_TEST((getIsomorphs(10) == V{5, 10}));
    BOOST_TEST((getIsomorphs(11) == V{7, 11, 13, 14}));
    BOOST_TEST((getIsomorphs(12) == V{3, 6, 9, 12}));
    BOOST_TEST((getIsomorphs(13) == V{7, 11, 13, 14}));
    BOOST_TEST((getIsomorphs(14) == V{7, 11, 13, 14}));
    BOOST_TEST((getIsomorphs(15) == V{15}));
}

BOOST_AUTO_TEST_SUITE_END() // FieldTest
