#include <iostream> 
#include <gmpxx.h>

int main() { 
    mpz_class e("65537", 10);
    mpz_class p("ecfc4ddf98ac14100230284ddf6f3a109995e7429407039985776681a702eedd25683888f090a6f87778aeed537170ef2901644a560ae76273fccc5b45aa9f97", 16);
    mpz_class q("edf5598b5a427b9c64ecd007e336e21eb3a93788b55c3f0cd137e2bbae554d35721b1fe6db65e9f0a23c3b963702ecb4fcab58882a0cdc161d1af571cf14a553", 16);
    mpz_class n = p*q;
    mpz_class c, m = 3;

    mpz_powm(c.get_mpz_t(), m.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());
    std::cout << c << std::endl;
}
