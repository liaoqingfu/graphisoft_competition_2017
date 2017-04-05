#include <iostream>

main() {
    int n;
    std::cin >> n;
    std::cout << (n & n - 1 ? n * 2 - (2 << __builtin_ilogb(n)) : n);
}
