#include "TestTools.hpp"
#include "Track.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(ToBox)

BOOST_AUTO_TEST_CASE(OneField) {
    Track track{1, 1, {8}, {}, {}};
    std::string tmp =
R"foo(
       0    
   ┏━━━━━━━┓
   ┃       ┗
0  ┃        
   ┃       ┏
   ┗━━━━━━━┛
)foo";
    std::string expected(tmp.begin() + 1, tmp.end());
    BOOST_CHECK_EQUAL(toBox(track), expected);
}

BOOST_AUTO_TEST_CASE(OneRow) {
    Track track{2, 1, {8, 2}, {}, {}};
    std::string tmp =
R"foo(
       0         1    
   ┏━━━━━━━┓ ┏━━━━━━━┓
   ┃       ┗━┛       ┃
0  ┃                 ┃
   ┃       ┏━┓       ┃
   ┗━━━━━━━┛ ┗━━━━━━━┛
)foo";
    std::string expected(tmp.begin() + 1, tmp.end());
    BOOST_CHECK_EQUAL(toBox(track), expected);
}

BOOST_AUTO_TEST_CASE(Test4x4) {
    Track track{2, 2, {12, 6, 1, 1}, {}, {}};
    std::string tmp =
R"foo(
       0         1    
   ┏━━━━━━━┓ ┏━━━━━━━┓
   ┃       ┗━┛       ┃
0  ┃                 ┃
   ┃       ┏━┓       ┃
   ┗━┓   ┏━┛ ┗━┓   ┏━┛
   ┏━┛   ┗━┓ ┏━┛   ┗━┓
   ┃       ┃ ┃       ┃
1  ┃       ┃ ┃       ┃
   ┃       ┃ ┃       ┃
   ┗━━━━━━━┛ ┗━━━━━━━┛
)foo";
    std::string expected(tmp.begin() + 1, tmp.end());
    BOOST_CHECK_EQUAL(toBox(track), expected);
}

BOOST_AUTO_TEST_SUITE_END() // ToBox
