#include <iostream>
#include <gmpxx.h>
int main() { 
    mpz_class x = 1;
    for (size_t i = 1; i < 50; i++) {
        x *= i; 
    }
    std::cout << x << std::endl;
}
