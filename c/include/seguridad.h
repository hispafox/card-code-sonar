#ifndef SEGURIDAD_H
#define SEGURIDAD_H

#include <stddef.h>
#include <stdint.h>

/*
 * Las contramedidas del código, juntas. Cualquier cambio aquí pasa por quien
 * responde de la seguridad del producto, y su ensamblador (gcc -Os -S) se
 * revisa cada vez que cambia la cadena de compilación.
 */

/*
 * CONTRAMEDIDA (fallos provocados): le dice a GCC que 'v' puede haber cambiado,
 * para que no junte dos comprobaciones seguidas del mismo valor. Es una extensión
 * de GCC; con otro compilador se escribe de otra forma.
 */
#define SEGURIDAD_BARRERA(v)  __asm__ __volatile__("" : "+r"(v) : : "memory")

/* CONTRAMEDIDA (canal lateral): cero solo si los n bytes coinciden; recorre siempre los n. */
uint8_t seguridad_diferencia(const uint8_t *a, const uint8_t *b, size_t n);

/* CONTRAMEDIDA (datos sensibles en memoria): pone a cero n bytes, sin que el compilador lo pueda quitar. */
void    seguridad_borrar(void *p, size_t n);

/* Lo que hace la tarjeta cuando detecta un ataque en curso: bloquea el PIN para siempre. */
void    tarjeta_bloquear(void);

#endif /* SEGURIDAD_H */
