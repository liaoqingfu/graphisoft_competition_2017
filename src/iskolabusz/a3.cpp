#include <iostream>

int j(int n) {
    return n > 1 ? (j(n - 1) + 2) % n + 1 : 1;
}

int main() {
    int N;
    std::cin >> N;
    std::cout << j(N);
}
