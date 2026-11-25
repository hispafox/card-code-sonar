#include "operaciones.h"

#include "memoria.h"
#include "utilidades.h"

#define OPERACIONES_DIRECCION  0x0090u
#define OPERACIONES_LONGITUD   4u

static uint32_t operaciones_totales;

uint32_t operaciones_leer(void)
{
    uint8_t bytes[OPERACIONES_LONGITUD];

    for (uint8_t i = 0u; i < OPERACIONES_LONGITUD; i++) {
        bytes[i] = memoria_leer((uint16_t)(OPERACIONES_DIRECCION + i));
    }
    return util_leer_u32(bytes);
}

memoria_estado operaciones_contar(void)
{
    uint8_t        bytes[OPERACIONES_LONGITUD];
    memoria_estado estado = MEMORIA_SIN_ERROR;

    operaciones_totales = operaciones_leer();
    operaciones_totales++;
    util_escribir_u32(bytes, operaciones_totales);
    for (uint8_t i = 0u; i < OPERACIONES_LONGITUD && estado == MEMORIA_SIN_ERROR; i++) {
        estado = memoria_escribir((uint16_t)(OPERACIONES_DIRECCION + i), bytes[i]);
    }
    return estado;
}
