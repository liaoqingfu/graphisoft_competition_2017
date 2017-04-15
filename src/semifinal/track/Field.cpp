#include "Field.hpp"

const std::array<std::string, numFieldTypes> fieldTypes {{
        " ", "╹", "╸", "┛", "╻", "┃", "┓", "┫",
        "╺", "┗", "━", "┻", "┏", "┣", "┳", "╋"}};

const std::array<std::string, numPlayers> playerColors {{
       "\e[0;31m", "\e[0;32m", "\e[0;33m", "\e[0;34m" }};

const std::string monitorColor = "\e[46m";
const std::string noColor = "\e[0m";

std::string to_string(const Field& field)
{
    std::string result;
    if (field.monitor) {
        result += monitorColor;
    }
    if (field.princess >= 0) {
        assert(field.princess <= static_cast<int>(numPlayers));
        result += playerColors[field.princess];
    }
    assert(field.type >= 0);
    assert(field.type < static_cast<int>(numFieldTypes));
    result += fieldTypes[field.type] + noColor;
    return result;
}

