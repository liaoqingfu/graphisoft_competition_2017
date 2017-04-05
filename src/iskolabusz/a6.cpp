#include <iostream>
#include <cmath>

int main() {
    int n;
    std::cin >> n;
    std::cout << ((n & (n - 1)) ? (n - (1<<((int)log2(n)))) * 2 : n);
}
