#include <iostream>

int recur(int N) {
    if (N == 1) return 1;
    if (N % 2) return (recur(N / 2 + 1) - 1) * 2;
    return recur(N / 2) * 2;
}

int main() {
    int N;
    std::cin >> N;
    // TODO check if we have to put \n , or maybe we could spare it
    std::cout << recur(N) << '\n';
}

