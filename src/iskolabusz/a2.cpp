#include <iostream>

int r(int N) {
    return N > 1 ? (N % 2) ? (r(N / 2 + 1) - 1) * 2
            : r(N / 2) * 2
            : 1;
}

int main() {
    int N;
    std::cin >> N;
    std::cout << r(N);
}
