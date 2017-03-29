#include <vector>
#include <numeric>
#include <cassert>

using namespace std;

pair<vector<int>, int> remaining_elements(const vector<int>& a, int offset) {
    vector<int> res;
    size_t i = offset;
    for (; i < a.size(); i+=2) {
        res.push_back(a[i]);
    }
    assert(i == a.size() || i == a.size() + 1);
    return {res, i - a.size()};
}

int brute(int N) {
    if (N == 1) { return 1; }

    vector<int> a(N);
    iota(a.begin(), a.end(), 1);
    auto rem = a;
    auto offset = 1;

    while (rem.size() > 1) {
        std::tie(rem, offset) = remaining_elements(rem, offset);
    }
    return rem[0];
}

#include <boost/test/unit_test.hpp>
BOOST_AUTO_TEST_SUITE( iskolabusz )

BOOST_AUTO_TEST_CASE(First) {
    BOOST_CHECK_EQUAL(brute(1), 1);
    BOOST_CHECK_EQUAL(brute(2), 2);
    BOOST_CHECK_EQUAL(brute(3), 2);
    BOOST_CHECK_EQUAL(brute(4), 4);
    BOOST_CHECK_EQUAL(brute(5), 2);
    BOOST_CHECK_EQUAL(brute(6), 4);
    BOOST_CHECK_EQUAL(brute(7), 6);
    BOOST_CHECK_EQUAL(brute(8), 8);
    BOOST_CHECK_EQUAL(brute(9), 2);
    BOOST_CHECK_EQUAL(brute(10), 4);
}

BOOST_AUTO_TEST_SUITE_END()
