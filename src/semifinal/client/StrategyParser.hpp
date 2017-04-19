#ifndef SEMIFINAL_CLIENT_STRATEGYPARSER_HPP
#define SEMIFINAL_CLIENT_STRATEGYPARSER_HPP

#include "ChoosingStrategy.hpp"

#include <string>
#include <random>

ChoosingStrategy parseStrategy(const std::string& input, std::mt19937& rng);

#endif // SEMIFINAL_CLIENT_STRATEGYPARSER_HPP
