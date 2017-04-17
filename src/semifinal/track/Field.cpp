#include "Field.hpp"

#include <algorithm>

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

std::string to_string(const Field& field)
{
    assert(field.princess <= static_cast<int>(numPlayers));
    assert(field.type >= 0);
    assert(field.type < static_cast<int>(numFieldTypes));
    return setColor(field.monitor >= 0 ? monitorColor : defaultColor,
                    field.princess >= 0 ? playerColors[field.princess]
                            : defaultColor)
            + fieldTypes[field.type]
            + clearColor();
}

const std::vector<int>& getIsomorphs(int fieldType) {
    assert(fieldType >= 1);
    assert(fieldType < static_cast<int>(numFieldTypes));
    return *isomorphCatalog[fieldType];
}

namespace BigFieldLines {
    constexpr static std::array<const char*, 2> upper{
        {"┏━━━━━━━┓", "┏━━━┻━━━┓"}};
    constexpr static std::array<const char*, 2> lower{
        {"┗━━━━━━━┛", "┗━━━┳━━━┛"}};
    constexpr static std::array<const char*, 2> left{
        { "┃","┫"}};
    constexpr static std::array<const char*, 2> right{
        { "┃","┣"}};
    constexpr static const char* neutral = "┃";
}

namespace {
auto getUpperLine(const Field& field) {
    return BigFieldLines::upper[0b0001 & field.type];
}
auto getLowerLine(const Field& field) {
    return BigFieldLines::lower[(0b0100 & field.type) >> 2];
}
std::string get2ndLine(const Field& field) {
    auto result = std::string(BigFieldLines::neutral);
    if (field.princess == -1) {
        result.append("       ");
    } else {
        result.append(" K").append(std::to_string(field.princess));
        result.append("    ");
    }
    return result.append(BigFieldLines::neutral);
}
auto getLeftChar(const Field& field) {
    return BigFieldLines::left[(0b0010 & field.type) >> 1];
}
auto getRightChar(const Field& field) {
    return BigFieldLines::right[(0b1000 & field.type) >> 3];
}
std::string getMiddleLine(const Field& field) {
    return std::string().append(getLeftChar(field)).append("       ").append(getRightChar(field));
}
std::string get4thLine(const Field& field) {
    auto result = std::string(BigFieldLines::neutral);
    if (field.monitor == -1) {
        result.append("       ");
    } else {
        result.append(" M").append(std::to_string(field.monitor));
        result.append("    ");
    }
    return result.append(BigFieldLines::neutral);
}
} // unnamed

std::string getBoxLine(const Field& field, unsigned i) {
    switch(i) {
        case 0:
            return getUpperLine(field);
        case 1:
            return get2ndLine(field);
        case 2:
            return getMiddleLine(field);
        case 3:
            return get4thLine(field);
        case 4:
            return getLowerLine(field);
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

