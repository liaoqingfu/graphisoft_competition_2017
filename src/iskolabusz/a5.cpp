#include <iostream>

main() {
    int n;
    std::cin >> n;
    std::cout << (n & n - 1 ? 2 * n - (2<<31 - __builtin_clz(n)) : n);
}
