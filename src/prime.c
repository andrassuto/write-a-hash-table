#include <math.h>

#include "prime.h"

/*
 * Returns:
 *   1  - prime
 *   0  - not prime
 *   -1 - undefined (i.e. x < 2)
 */
int is_prime(const int x) {
    if (x < 2) {
        return -1;
    }
    if (x % 2 == 0) {
        return 0;
    }
    for (int i = 2; i < floor(sqrt(x)); i++) {
        if (x % i == 0) {
            return 0;
        }
    }
    return 1;
}

int next_prime(int x) {
    while (is_prime(x) != 1) {
        x++;
    }
    return x;
}
