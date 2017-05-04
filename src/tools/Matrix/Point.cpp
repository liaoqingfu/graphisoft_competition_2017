#include "Point.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <cassert>

std::ostream& operator<<(std::ostream& os, Point p) {
    os << '(' << p.x << ", " << p.y << ')';
    return os;
}

Point parsePoint(const std::string& s) {
    namespace ascii = boost::spirit::ascii;
    namespace qi = boost::spirit::qi;
    namespace phoenix = boost::phoenix;
    using qi::int_;
    using qi::_1;
    using phoenix::ref;

    Point p{0, 0};
    auto begin = s.begin();
    auto end = s.end();
    bool success = qi::phrase_parse(begin, end,
            int_[ref(p.x) = _1] >> "," >> int_[ref(p.y) = _1], ascii::space);

    if (!success) {
        std::cerr << "Parse error\n";
        throw std::logic_error{"Parse error"};
    }
    if (begin != end) {
        std::cerr << "Parse error at\n";
        std::copy(begin, end, std::ostream_iterator<char>(std::cerr));
        std::cerr << "\n";
        throw std::logic_error{"Parse error"};
    }
    return p;
}

std::istream& operator>>(std::istream& os, Point& p) {
    std::string s;
    os >> s;
    p = parsePoint(s);
    return os;
}

Direction toDirection(const Point& source, const Point& destination) {
    Point diff = destination - source;
    if (diff == p10) {
        return Direction::right;
    } else if (diff == p01) {
        return Direction::down;
    } else if (diff == p11) {
        assert(false);
    } else if (diff*-1 == p10) {
        return Direction::left;
    } else if (diff*-1 == p01) {
        return Direction::up;
    } else if (diff*-1 == p11) {
        assert(false);
    } else {
        assert(false);
    }
    assert(false);
}
