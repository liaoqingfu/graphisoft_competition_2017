#include <iostream>

int main() {
    int n;
    std::cin >> n;
    std::cout << ((n & (n - 1)) ? (n - (1<<(31 - __builtin_clz(n)))) * 2 : n);
}
