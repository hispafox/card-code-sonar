#ifndef EJEMPLOS_H
#define EJEMPLOS_H

#include <stdint.h>

/*
 * Las dos funciones de demostración del submódulo 3.4: lo que hace el
 * optimizador con una comprobación que depende de un comportamiento indefinido.
 * Su ensamblador se ve con gcc -Os -S sobre ejemplos.c.
 */
int32_t sumar_cien(int32_t total);
int32_t leer_primero(const int32_t *p);

#endif /* EJEMPLOS_H */
