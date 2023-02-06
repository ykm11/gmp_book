#include <iostream>
#include <gmpxx.h>

int main() {
    mpq_class a(1, 17);
#if 1
    mpf_class x(a, 128);
#else
    mpf_class x(a);
#endif

    printf("x->_mp_size = %d\n", (x.get_mpf_t())->_mp_size);
    printf("x->_mp_prec = %d\n", (x.get_mpf_t())->_mp_prec);
    printf("x->_mp_exp = %ld\n", (x.get_mpf_t())->_mp_exp);

    gmp_printf("%.60Ff\n", x.get_mpf_t());

}
