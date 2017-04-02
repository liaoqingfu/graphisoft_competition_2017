#include <iostream>

int main() {
    int N;
    std::cin >> N;
    std::cout << ((N > 1 ? (N - (1 << (32 - __builtin_clz(N - 1) - 1))) << 1
                    : 1));
}
