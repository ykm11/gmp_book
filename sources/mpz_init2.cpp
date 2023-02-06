#include <iostream>
#include <gmp.h>

void mpz_dump(const mpz_t r) {
    printf("_mp_alloc = %d\n", r->_mp_alloc);
    printf("_mp_size  = %d\n", r->_mp_size);
    printf("_mp_d     = %p\n", r->_mp_d);
}


int main() {
    mpz_t x;
    mpz_init2(x, 256);
    mpz_dump(x);
}
