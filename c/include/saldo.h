#ifndef SALDO_H
#define SALDO_H

#include <stdint.h>

#include "memoria.h"
#include "utilidades.h"

/* El saldo ocupa dos bytes de la memoria persistente, el alto primero, y detrás va su CRC. */
#define SALDO_LONGITUD           2u
#define SALDO_GUARDADO_LONGITUD  (SALDO_LONGITUD + 2u)

/* Deja el saldo en '*saldo' solo si cuadra con su CRC; si no, devuelve MEMORIA_DATOS_CORROMPIDOS. */
UTIL_NO_IGNORAR memoria_estado saldo_leer(uint16_t *saldo);
UTIL_NO_IGNORAR memoria_estado saldo_escribir(uint16_t saldo);

#endif /* SALDO_H */
