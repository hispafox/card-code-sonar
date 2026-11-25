#include "seguridad.h"

#include "pin.h"

/* CONTRAMEDIDA (canal lateral): recorre siempre todos los bytes; el tiempo no depende de cuántos coinciden. */
uint8_t seguridad_diferencia(const uint8_t *a, const uint8_t *b, size_t n)
{
    uint8_t diferencia = 0u;

    for (size_t i = 0u; i < n; i++) {
        diferencia |= (uint8_t)(a[i] ^ b[i]);
    }
    return diferencia;                      /* cero solo si los dos son iguales */
}

/* CONTRAMEDIDA (datos sensibles en memoria): escribe a través de un puntero volatile para que el borrado no se pueda quitar. */
void seguridad_borrar(void *p, size_t n)
{
    volatile uint8_t *v = (volatile uint8_t *)p;

    while (n-- > 0u) {
        *v++ = 0u;
    }
}

void tarjeta_bloquear(void)
{
    pin_bloquear();
}
