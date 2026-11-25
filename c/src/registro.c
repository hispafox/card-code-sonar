#include "registro.h"

#include "crc.h"
#include "memoria.h"
#include "utilidades.h"

#define REGISTRO_DIRECCION  0x0080u
#define REGISTRO_LONGITUD   6u    /* tres valores de dos bytes */
#define REGISTRO_CUENTA_DIRECCION  0x0088u   /* la cuenta de pagos, un byte, y su CRC */

static memoria_estado registro_volcar(const uint8_t *bytes, uint8_t n)
{
    memoria_estado estado = MEMORIA_SIN_ERROR;

    for (uint8_t i = 0u; i < n && estado == MEMORIA_SIN_ERROR; i++) {
        estado = memoria_escribir((uint16_t)(REGISTRO_DIRECCION + i), bytes[i]);
    }
    return estado;
}

/* Suma uno a la cuenta de pagos; sin cuenta válida, empieza en uno. */
static memoria_estado registro_sumar_pago(void)
{
    uint8_t        bytes[3];
    uint8_t        total = 0u;
    memoria_estado estado = MEMORIA_SIN_ERROR;

    if (registro_leer_cuenta(&total) != MEMORIA_SIN_ERROR) {
        total = 0u;
    }
    bytes[0] = (uint8_t)(total + 1u);
    util_escribir_u16(&bytes[1], crc_calcular(bytes, 1u));
    for (uint8_t i = 0u; i < sizeof bytes && estado == MEMORIA_SIN_ERROR; i++) {
        estado = memoria_escribir((uint16_t)(REGISTRO_CUENTA_DIRECCION + i), bytes[i]);
    }
    return estado;
}

memoria_estado registro_guardar_pago(const registro_pago *pago)
{
    uint8_t bufer[REGISTRO_LONGITUD + 2u];
    uint8_t longitud = REGISTRO_LONGITUD;
    uint16_t crc;
    memoria_estado estado;

    util_escribir_u16(&bufer[0], pago->importe);
    util_escribir_u16(&bufer[2], pago->saldo_anterior);
    util_escribir_u16(&bufer[4], pago->saldo_nuevo);
    crc = crc_calcular(bufer, longitud);
    util_escribir_u16(&bufer[REGISTRO_LONGITUD], crc);
    estado = registro_volcar(bufer, sizeof bufer);
    if (estado == MEMORIA_SIN_ERROR) {
        estado = registro_sumar_pago();
    }
    return estado;
}

memoria_estado registro_leer_pago(registro_pago *pago)
{
    uint8_t bufer[REGISTRO_LONGITUD + 2u];

    for (uint8_t i = 0u; i < sizeof bufer; i++) {
        bufer[i] = memoria_leer((uint16_t)(REGISTRO_DIRECCION + i));
    }
    if (crc_calcular(bufer, REGISTRO_LONGITUD) != util_leer_u16(&bufer[REGISTRO_LONGITUD])) {
        return MEMORIA_DATOS_CORROMPIDOS;
    }
    pago->importe = util_leer_u16(&bufer[0]);
    pago->saldo_anterior = util_leer_u16(&bufer[2]);
    pago->saldo_nuevo = util_leer_u16(&bufer[4]);
    return MEMORIA_SIN_ERROR;
}

uint8_t registro_hay_pago(void)
{
    registro_pago pago;

    return (uint8_t)(registro_leer_pago(&pago) == MEMORIA_SIN_ERROR);
}

memoria_estado registro_leer_cuenta(uint8_t *total)
{
    uint8_t bytes[3];

    for (uint8_t i = 0u; i < sizeof bytes; i++) {
        bytes[i] = memoria_leer((uint16_t)(REGISTRO_CUENTA_DIRECCION + i));
    }
    if (crc_calcular(bytes, 1u) != util_leer_u16(&bytes[1])) {
        return MEMORIA_DATOS_CORROMPIDOS;
    }
    *total = bytes[0];
    return MEMORIA_SIN_ERROR;
}

memoria_estado registro_contar(uint8_t *total)
{
    memoria_estado estado = MEMORIA_DATOS_CORROMPIDOS;   /* si no hay pago, no hay cuenta */

    if (registro_hay_pago()) {
        estado = registro_leer_cuenta(total);
    }
    return estado;
}
