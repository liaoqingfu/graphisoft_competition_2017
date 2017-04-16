#include "Field.hpp"

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
