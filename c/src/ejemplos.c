#include "ejemplos.h"

#include <stddef.h>

int32_t sumar_cien(int32_t total)
{
    if (total > INT32_MAX - 100) {
        return -1;
    }
    return total + 100;
}

int32_t leer_primero(const int32_t *p)
{
    if (p == NULL) {
        return 0;
    }
    return *p;
}
