#include "datos.h"

#include "memoria.h"

#define DATOS_PUBLICOS_DIRECCION  0x00C0u

size_t datos_publicos_leer(uint8_t *destino, size_t capacidad)
{
    size_t n = (capacidad < DATOS_PUBLICOS_LONGITUD) ? capacidad : DATOS_PUBLICOS_LONGITUD;

    for (size_t i = 0u; i < n; i++) {
        destino[i] = memoria_leer((uint16_t)(DATOS_PUBLICOS_DIRECCION + i));
    }
    return n;
}
