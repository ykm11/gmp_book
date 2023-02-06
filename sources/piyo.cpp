#include <iostream>
#include <gmpxx.h>

void dump_mpf(const mpf_t r) {
    printf("_mp_size = %d\n", r->_mp_size);
    printf("_mp_prec = %d\n", r->_mp_prec);
}

int main() {
    mpf_t x;
    //mpf_set_default_prec(200);
    mpf_init(x);
    //mpf_set_prec(x, 10);
    dump_mpf(x);
}
