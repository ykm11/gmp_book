#include <iostream>
#include <gmpxx.h>

// p = 36 * t^4 + 36 * t^3 + 24 * t^2 + 6 * t + 1

int main() {
    mpz_class r;
    mpz_class z, s, t;
    gmp_randclass r1(gmp_randinit_default);
    r = r1.get_z_bits(64);
 
    for (size_t i = 0; i < 2000; i++) {
        s = r + 1;
        z = r*r;  // r^2
        t = z*r; // r^3
        t *= 36*s; // 36r^3 * (r + 1) = 36 * r^4 + 36 * r^3
        t += 24*z; // 36 * r^4 + 36 * r^3 + 24*r^2
        t += 6*r + 1;

        if (mpz_probab_prime_p(t.get_mpz_t(), 20)) {
            std::cout << r << std::endl;
            std::cout << t << std::endl;
            std::cout << (t % 3)<< std::endl;
            std::cout << mpz_sizeinbase(t.get_mpz_t(), 2) << std::endl;
            break;
        }
        r++;
    }
}
