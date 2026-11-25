#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdint.h>

#include "memoria.h"
#include "utilidades.h"

/* El último pago, tal como queda anotado en la memoria persistente, con su CRC. */
typedef struct {
    uint16_t importe;
    uint16_t saldo_anterior;
    uint16_t saldo_nuevo;
} registro_pago;

UTIL_NO_IGNORAR memoria_estado registro_guardar_pago(const registro_pago *pago);
/* Deja el último pago en '*pago' solo si cuadra con su CRC; si no, devuelve MEMORIA_DATOS_CORROMPIDOS. */
memoria_estado registro_leer_pago(registro_pago *pago);

/* 1 si hay algún pago anotado. */
uint8_t        registro_hay_pago(void);
/* Deja en '*total' cuántos pagos se han anotado, solo si la cuenta cuadra con su CRC. */
memoria_estado registro_leer_cuenta(uint8_t *total);
/* Cuántos pagos se han anotado; sin ninguno, MEMORIA_DATOS_CORROMPIDOS. */
memoria_estado registro_contar(uint8_t *total);

#endif /* REGISTRO_H */
