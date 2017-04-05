#include <iostream>

int main() {
    int n;
    std::cin >> n;
    std::cout << ((n & (n - 1)) ? (n * 2 - (1<<fls(n))) : n);
}
