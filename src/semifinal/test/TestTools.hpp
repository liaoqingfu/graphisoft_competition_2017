#ifndef SEMIFINAL_TEST_TESTTOOLS_HPP
#define SEMIFINAL_TEST_TESTTOOLS_HPP

#include <algorithm>
#include <functional>

template<typename Container,
        typename Predicate = std::less<typename Container::value_type>>
Container sorted(const Container& container,
        const Predicate& predicate = Predicate{}) {
    Container result{container};
    using std::begin;
    using std::end;
    std::sort(begin(result), end(result), predicate);
    return result;
}

#endif // SEMIFINAL_TEST_TESTTOOLS_HPP
