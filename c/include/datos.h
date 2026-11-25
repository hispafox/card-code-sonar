#ifndef DATOS_H
#define DATOS_H

#include <stddef.h>
#include <stdint.h>

/* Los datos públicos de la tarjeta, grabados en fábrica en la memoria persistente. */
#define DATOS_PUBLICOS_LONGITUD  300u

/* Copia en 'destino' los datos públicos, hasta 'capacidad' bytes, y devuelve cuántos ha copiado. */
size_t datos_publicos_leer(uint8_t *destino, size_t capacidad);

#endif /* DATOS_H */
