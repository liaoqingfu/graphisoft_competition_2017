#include <vector>
#include <numeric>
#include <cassert>
#include <cmath>

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

int recur(int N) {
    if (N == 1) return 1;
    if (N % 2) return (recur(N / 2 + 1) - 1) * 2;
    return recur(N / 2) * 2;
}

//http://stackoverflow.com/questions/53161/find-the-highest-order-bit-in-c
//http://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2
int xx(int n) {
    //return (n & (n - 1)) ? (n - (1<<(fls(n)-1))) * 2 : n;
    //return ((n & (n - 1)) ? (n * 2 - (1<<fls(n))) : n);
    //return ((n & (n - 1)) ? (n - (1<<((int)log2(n)))) * 2 : n);
    return ((n & (n - 1)) ? (n - (1<<(31 - __builtin_clz(n)))) * 2 : n);
}



#include <boost/test/unit_test.hpp>
#include <random>
BOOST_AUTO_TEST_SUITE( iskolabusz )

BOOST_AUTO_TEST_CASE(X) {
    BOOST_CHECK_EQUAL(xx(2), 2);
    BOOST_CHECK_EQUAL(xx(3), 2);
    BOOST_CHECK_EQUAL(xx(4), 4);
    BOOST_CHECK_EQUAL(xx(5), 2);
    BOOST_CHECK_EQUAL(xx(6), 4);
    BOOST_CHECK_EQUAL(xx(7), 6);
    BOOST_CHECK_EQUAL(xx(8), 8);
    BOOST_CHECK_EQUAL(xx(9), 2);
    BOOST_CHECK_EQUAL(xx(10), 4);
}

BOOST_AUTO_TEST_CASE(Brute) {
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

BOOST_AUTO_TEST_CASE(Recur) {
    BOOST_CHECK_EQUAL(recur(1), 1);
    BOOST_CHECK_EQUAL(recur(2), 2);
    BOOST_CHECK_EQUAL(recur(3), 2);
    BOOST_CHECK_EQUAL(recur(4), 4);
    BOOST_CHECK_EQUAL(recur(5), 2);
    BOOST_CHECK_EQUAL(recur(6), 4);
    BOOST_CHECK_EQUAL(recur(7), 6);
    BOOST_CHECK_EQUAL(recur(8), 8);
    BOOST_CHECK_EQUAL(recur(9), 2);
    BOOST_CHECK_EQUAL(recur(10), 4);
}

BOOST_AUTO_TEST_CASE(Validate_recursive_with_brute) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000000);

    for (int n=0; n<10; ++n) {
        auto x = dis(gen);
        BOOST_CHECK_EQUAL(recur(x), brute(x));
    }
}

BOOST_AUTO_TEST_SUITE_END()
