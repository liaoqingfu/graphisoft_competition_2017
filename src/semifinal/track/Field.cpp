#include "Field.hpp"

#include <algorithm>

namespace {

constexpr std::size_t numFieldTypes = 16;

const std::array<std::string, numFieldTypes> fieldTypes {{
        " ", "╹", "╸", "┛", "╻", "┃", "┓", "┫",
        "╺", "┗", "━", "┻", "┏", "┣", "┳", "╋"}};

constexpr int defaultColor = 0;

constexpr std::size_t numPlayers = 4;
constexpr std::array<int, numPlayers> playerColors {{31, 32, 33, 34}};

constexpr int monitorColor = 46;

std::string setColor(int background, int foreground) {
    using std::to_string;
    std::string result = "\e[" + to_string(background);
    if (foreground != defaultColor) {
        result += ";" + to_string(foreground);
    }
    result += "m";
    return result;
}

std::string clearColor() {
    return "\e[0m";
}

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

}

std::string to_string(const Field& field)
{
    assert(field.princess <= static_cast<int>(numPlayers));
    assert(field.type >= 0);
    assert(field.type < static_cast<int>(numFieldTypes));
    return setColor(field.monitor ? monitorColor : defaultColor,
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
