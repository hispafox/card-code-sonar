#ifndef EXTRACTO_H
#define EXTRACTO_H

#include <stdint.h>

typedef enum {
    EXTRACTO_PAGO = 1,
    EXTRACTO_RECARGA = 2,
    EXTRACTO_DEVOLUCION = 3
} extracto_tipo;

/* Un movimiento del registro, con el importe en centimos. */
typedef struct {
    uint8_t  tipo;      /* un extracto_tipo */
    uint16_t importe;
} extracto_movimiento;

#define EXTRACTO_MAX_MOVIMIENTOS  8u

/* Escriben en 'respuesta' el extracto de los 'n' movimientos, la segunda con los totales.
   Devuelven cuantos bytes han escrito, o 0 si no caben o hay un tipo desconocido. */
uint16_t extracto_generar(const extracto_movimiento *movimientos, uint8_t n,
                          uint8_t *respuesta, uint16_t tam);
uint16_t extracto_generar_con_totales(const extracto_movimiento *movimientos, uint8_t n,
                                      uint8_t *respuesta, uint16_t tam);

#endif /* EXTRACTO_H */
