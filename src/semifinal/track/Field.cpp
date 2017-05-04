#include "Field.hpp"

#include <boost/algorithm/string/join.hpp>

#include <algorithm>
#include <iostream>
#include <memory>

namespace {

std::vector<std::vector<int>> isomorphs{
        {1, 2, 4, 8},
        {3, 6, 9, 12},
        {7, 11, 13, 14},
        {5, 10},
        {15}};

std::vector<std::vector<int>*> calculateIsomorphs() {
    std::vector<std::vector<int>*> result{nullptr};
    for (int i = 1; i < static_cast<int>(numFieldTypes); ++i) {
        auto iterator = std::find_if(isomorphs.begin(), isomorphs.end(),
                [i](const std::vector<int>& v) {
                    return std::find(v.begin(), v.end(), i) != v.end();
                });
        result.push_back(&*iterator);
    }
    assert(result.size() == numFieldTypes);
    return result;
}

std::vector<std::vector<int>*> isomorphCatalog = calculateIsomorphs();

} // unnamed namespace

const std::array<std::string, numFieldTypes> fieldTypes {{
        " ", "╹", "╸", "┛", "╻", "┃", "┓", "┫",
        "╺", "┗", "━", "┻", "┏", "┣", "┳", "╋"}};

std::string setColor(int background, int foreground) {
    using std::to_string;
    std::string result = "\e[" + to_string(background);
    if (foreground != defaultColor) {
        result += ";" + to_string(foreground);
    }
    result += "m";
    return result;
}

const std::vector<int>& getIsomorphs(int fieldType) {
    assert(fieldType >= 1);
    assert(fieldType < static_cast<int>(numFieldTypes));
    return *isomorphCatalog[fieldType];
}

namespace BigFieldLines {
    using Array = std::array<const char*, 2>;
    constexpr static Array upper(bool blocked) {
        return blocked
                ? Array{{"┏╍╍╍╍╍╍╍┓", "┏╍┛   ┗╍┓"}}
                : Array{{"┏━━━━━━━┓", "┏━┛   ┗━┓"}};
    }
    constexpr static Array lower(bool blocked) {
        return blocked
                ? Array{{"┗╍╍╍╍╍╍╍┛", "┗╍┓   ┏╍┛"}}
                : Array{{"┗━━━━━━━┛", "┗━┓   ┏━┛"}};
    }

    namespace left {
        constexpr static Array u(bool blocked) {
            return blocked ? Array{{ "╏","┛"}} : Array{{ "┃","┛"}};
        };

        constexpr static Array m(bool blocked) {
            return blocked ? Array{{ "╏"," "}} : Array{{ "┃"," "}};
        };

        constexpr static Array d(bool blocked) {
            return blocked ? Array{{ "╏","┓"}} : Array{{ "┃","┓"}};
        };
    } // left

    namespace right {
        constexpr static Array u(bool blocked) {
            return blocked ? Array{{ "╏","┗"}} : Array{{ "┃","┗"}};
        };

        constexpr static Array m(bool blocked) {
            return blocked ? Array{{ "╏"," "}} : Array{{ "┃"," "}};
        };

        constexpr static Array d(bool blocked) {
            return blocked ? Array{{ "╏","┏"}} : Array{{ "┃","┏"}};
        };
    } // right

} // BigFieldLines

namespace {
auto getLeftUChar(const Field& field, bool blocked) {
    return BigFieldLines::left::u(blocked)[(0b0010 & field.type) >> 1];
}
auto getLeftMChar(const Field& field, bool blocked) {
    return BigFieldLines::left::m(blocked)[(0b0010 & field.type) >> 1];
}
auto getLeftDChar(const Field& field, bool blocked) {
    return BigFieldLines::left::d(blocked)[(0b0010 & field.type) >> 1];
}
auto getRightUChar(const Field& field, bool blocked) {
    return BigFieldLines::right::u(blocked)[(0b1000 & field.type) >> 3];
}
auto getRightMChar(const Field& field, bool blocked) {
    return BigFieldLines::right::m(blocked)[(0b1000 & field.type) >> 3];
}
auto getRightDChar(const Field& field, bool blocked) {
    return BigFieldLines::right::d(blocked)[(0b1000 & field.type) >> 3];
}

auto getUpperLine(const Field& field, bool blocked) {
    return BigFieldLines::upper(blocked)[0b0001 & field.type];
}
auto getLowerLine(const Field& field, bool blocked) {
    return BigFieldLines::lower(blocked)[(0b0100 & field.type) >> 2];
}

bool isBlocked(const boost::optional<ColorInfo>& colorInfo) {
    return colorInfo ? colorInfo->blocked : false;
}

template<typename ColorGetter>
std::unique_ptr<ColorDrawer> makeColorDrawer(
        std::string& line,
        const boost::optional<ColorInfo>& colorInfo,
        const ColorGetter& colorGetter) {
    if (!colorInfo) {
        return nullptr;
    }
    return std::make_unique<ColorDrawer>(line, colorGetter(*colorInfo),
            colorInfo->baseColor);
}

std::string getPrincesses(const Field field, unsigned begin, unsigned end,
        const boost::optional<ColorInfo>& colorInfo) {
    std::size_t size = 0;
    std::string result;
    for (unsigned princess = begin, to = 0; princess < end; ++princess, ++to) {
        if (!field.hasPrincess(princess)) {
            continue;
        }
        auto colorizer = makeColorDrawer(result, colorInfo,
                [princess](const ColorInfo& info) {
                    return info.princessColors[princess];
                });
        std::string s = "K" + std::to_string(princess);
        size += s.size() + 1;
        result.append(" ").append(s);
    }
    for (std::size_t j = 0; j < 7 - size; ++j) {
        result.append(" ");
    }
    return result;
}

std::string get2ndLine(const Field& field,
        const boost::optional<ColorInfo>& colorInfo) {
    auto result = std::string(getLeftUChar(field, isBlocked(colorInfo)));
    if (field.noPrincess()) {
        result.append("       ");
    } else {
        // Add the first two princesses
        result.append(getPrincesses(field, 0, 2, colorInfo));
    }
    return result.append(getRightUChar(field, isBlocked(colorInfo)));
}

std::string getMiddleLine(const Field& field,
        const boost::optional<ColorInfo>& colorInfo) {
    auto result = std::string(getLeftMChar(field, isBlocked(colorInfo)));
    if (field.noPrincess()) {
        result.append("       ");
    } else {
        // Add the last two princesses if there is any
        result.append(getPrincesses(field, 2, 4, colorInfo));
    }
    return result.append(getRightMChar(field, isBlocked(colorInfo)));
}

std::string get4thLine(const Field& field,
        const boost::optional<ColorInfo>& colorInfo) {
    auto result = std::string(getLeftDChar(field, isBlocked(colorInfo)));
    if (field.monitor == -1) {
        result.append("       ");
    } else {
        std::string Mstr;
        Mstr.append(" M").append(std::to_string(field.monitor));
        {
            auto colorizer = makeColorDrawer(result, colorInfo,
                    [](const ColorInfo& info) { return info.monitorColor; });
            result.append(Mstr);
        }
        if (colorInfo) {
            result.append(colorInfo->baseColor);
        }
        for (std::size_t j = 0; j < 7 - Mstr.size(); ++j) {
            result.append(" ");
        }
    }
    return result.append(getRightDChar(field, isBlocked(colorInfo)));
}

} // unnamed

std::string getBoxLine(const Field& field, unsigned i,
        const boost::optional<ColorInfo>& colorInfo) {
    switch(i) {
        case 0:
            return getUpperLine(field, isBlocked(colorInfo));
        case 1:
            return get2ndLine(field, colorInfo);
        case 2:
            return getMiddleLine(field, colorInfo);
        case 3:
            return get4thLine(field, colorInfo);
        case 4:
            return getLowerLine(field, isBlocked(colorInfo));
        default: return "";
    }
}

std::string toBox(const Field& field) {
    std::string result;
    for (int i = 0; i < BOXHEIGHT; ++i) {
        result.append(getBoxLine(field, i)).append("\n");
    }
    return result;
}

