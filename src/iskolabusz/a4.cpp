#include <iostream>

int main() {
    int n;
    std::cin >> n;
    std::cout << ((n & (n - 1)) ? (n - (1<<(fls(n)-1))) * 2 : n);
}
