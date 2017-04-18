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

BOOST_AUTO_TEST_SUITE(ToBox)

BOOST_AUTO_TEST_CASE(Value1) {
    Field field; field.type = 1;
    std::string tmp =
R"foo(
┏━┛   ┗━┓
┃       ┃
┃       ┃
┃       ┃
┗━━━━━━━┛
)foo";
    std::string expected(tmp.begin() + 1, tmp.end());
    BOOST_CHECK_EQUAL(toBox(field), expected);
}

BOOST_AUTO_TEST_CASE(Value2) {
    Field field; field.type = 2;
    std::string tmp =
R"foo(
┏━━━━━━━┓
┛       ┃
        ┃
┓       ┃
┗━━━━━━━┛
)foo";
    std::string expected(tmp.begin() + 1, tmp.end());
    BOOST_CHECK_EQUAL(toBox(field), expected);
}

BOOST_AUTO_TEST_CASE(Value4) {
    Field field; field.type = 4;
    std::string tmp =
R"foo(
┏━━━━━━━┓
┃       ┃
┃       ┃
┃       ┃
┗━┓   ┏━┛
)foo";
    std::string expected(tmp.begin() + 1, tmp.end());
    BOOST_CHECK_EQUAL(toBox(field), expected);
}

BOOST_AUTO_TEST_CASE(Value8) {
    Field field; field.type = 8;
    std::string tmp =
R"foo(
┏━━━━━━━┓
┃       ┗
┃        
┃       ┏
┗━━━━━━━┛
)foo";
    std::string expected(tmp.begin() + 1, tmp.end());
    BOOST_CHECK_EQUAL(toBox(field), expected);
}

BOOST_AUTO_TEST_CASE(Value15) {
    Field field; field.type = 15;
    std::string tmp =
R"foo(
┏━┛   ┗━┓
┛       ┗
         
┓       ┏
┗━┓   ┏━┛
)foo";
    std::string expected(tmp.begin() + 1, tmp.end());
    BOOST_CHECK_EQUAL(toBox(field), expected);
}

BOOST_AUTO_TEST_CASE(Princess) {
    Field field; field.type = 8;
    field.addPrincess(3);
    std::string tmp =
R"foo(
┏━━━━━━━┓
┃ K3    ┗
┃        
┃       ┏
┗━━━━━━━┛
)foo";
    std::string expected(tmp.begin() + 1, tmp.end());
    BOOST_CHECK_EQUAL(toBox(field), expected);
}

BOOST_AUTO_TEST_CASE(Monitor) {
    Field field; field.type = 8;
    field.monitor = 3;
    std::string tmp =
R"foo(
┏━━━━━━━┓
┃       ┗
┃        
┃ M3    ┏
┗━━━━━━━┛
)foo";
    std::string expected(tmp.begin() + 1, tmp.end());
    BOOST_CHECK_EQUAL(toBox(field), expected);
}



BOOST_AUTO_TEST_SUITE_END() // ToBox

BOOST_AUTO_TEST_SUITE_END() // FieldTest
