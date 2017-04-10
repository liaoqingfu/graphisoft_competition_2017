#ifndef LAKE_ERROR_HPP
#define LAKE_ERROR_HPP

#include <cstdlib>

enum class Error {
    NoRemovebleFerries = 10,
    NoAddableFerries = 11,
    NoSolution = 12,
    WrongSolution = 13,
    SolutionTooLong = 14,
};

void throwError(Error error) {
    std::exit(static_cast<int>(error));
}


#endif // LAKE_ERROR_HPP
