#pragma once

#include <array>

struct OpponentData {
    int extraField;
};

using OpponentsInfo = std::array<OpponentData, 4>;
