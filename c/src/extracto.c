#include "extracto.h"

#include "utilidades.h"

#define EXTRACTO_VERSION            0x01u
#define EXTRACTO_LONGITUD_CABECERA  2u    /* la version y el numero de movimientos */
#define EXTRACTO_LONGITUD_LINEA     3u    /* la letra del tipo y el importe */
#define EXTRACTO_LONGITUD_IMPORTE   2u
#define EXTRACTO_LONGITUD_SUMA      1u

/* La letra de cada tipo en el extracto; el indice es el tipo. Un tipo nuevo es una fila mas. */
static const uint8_t extracto_letras[] = {
    [EXTRACTO_PAGO]       = 'P',
    [EXTRACTO_RECARGA]    = 'R',
    [EXTRACTO_DEVOLUCION] = 'D',
};

#define EXTRACTO_TIPOS             (UTIL_ELEMENTOS(extracto_letras) - 1u)   /* la fila 0 no es un tipo */
#define EXTRACTO_LONGITUD_TOTALES  (EXTRACTO_TIPOS * EXTRACTO_LONGITUD_IMPORTE)

static int extracto_tipo_valido(uint8_t tipo)
{
    return tipo < UTIL_ELEMENTOS(extracto_letras) && extracto_letras[tipo] != 0u;
}

static uint16_t extracto_longitud(uint8_t n)
{
    return (uint16_t)(EXTRACTO_LONGITUD_CABECERA + EXTRACTO_LONGITUD_LINEA * n + EXTRACTO_LONGITUD_SUMA);
}

static int extracto_se_puede_generar(const extracto_movimiento *movimientos, uint8_t n,
                                     uint16_t necesarios, uint16_t tam)
{
    if (n > EXTRACTO_MAX_MOVIMIENTOS || tam < necesarios) {
        return 0;
    }
    for (uint8_t i = 0u; i < n; i++) {
        if (!extracto_tipo_valido(movimientos[i].tipo)) {
            return 0;
        }
    }
    return 1;
}

static uint16_t extracto_escribir_movimientos(const extracto_movimiento *movimientos, uint8_t n,
                                              uint8_t *respuesta)
{
    uint16_t posicion = 0u;

    respuesta[posicion++] = EXTRACTO_VERSION;
    respuesta[posicion++] = n;
    for (uint8_t i = 0u; i < n; i++) {
        respuesta[posicion++] = extracto_letras[movimientos[i].tipo];
        util_escribir_u16(&respuesta[posicion], movimientos[i].importe);
        posicion = (uint16_t)(posicion + EXTRACTO_LONGITUD_IMPORTE);
    }
    return posicion;
}

static uint16_t extracto_escribir_totales(const extracto_movimiento *movimientos, uint8_t n,
                                          uint8_t *destino)
{
    uint16_t totales[UTIL_ELEMENTOS(extracto_letras)] = {0u};
    uint16_t posicion = 0u;

    for (uint8_t i = 0u; i < n; i++) {
        totales[movimientos[i].tipo] = (uint16_t)(totales[movimientos[i].tipo] + movimientos[i].importe);
    }
    for (uint8_t tipo = EXTRACTO_PAGO; tipo <= EXTRACTO_TIPOS; tipo++) {
        util_escribir_u16(&destino[posicion], totales[tipo]);
        posicion = (uint16_t)(posicion + EXTRACTO_LONGITUD_IMPORTE);
    }
    return posicion;
}

/* Anade la suma de control, el o exclusivo de todo lo anterior, y devuelve la longitud final. */
static uint16_t extracto_cerrar(uint8_t *respuesta, uint16_t longitud)
{
    uint8_t suma = 0u;

    for (uint16_t i = 0u; i < longitud; i++) {
        suma = (uint8_t)(suma ^ respuesta[i]);
    }
    respuesta[longitud] = suma;
    return (uint16_t)(longitud + EXTRACTO_LONGITUD_SUMA);
}

uint16_t extracto_generar(const extracto_movimiento *movimientos, uint8_t n,
                          uint8_t *respuesta, uint16_t tam)
{
    if (!extracto_se_puede_generar(movimientos, n, extracto_longitud(n), tam)) {
        return 0u;
    }
    return extracto_cerrar(respuesta, extracto_escribir_movimientos(movimientos, n, respuesta));
}

uint16_t extracto_generar_con_totales(const extracto_movimiento *movimientos, uint8_t n,
                                      uint8_t *respuesta, uint16_t tam)
{
    uint16_t necesarios = (uint16_t)(extracto_longitud(n) + EXTRACTO_LONGITUD_TOTALES);
    uint16_t longitud;

    if (!extracto_se_puede_generar(movimientos, n, necesarios, tam)) {
        return 0u;
    }
    longitud = extracto_escribir_movimientos(movimientos, n, respuesta);
    longitud = (uint16_t)(longitud + extracto_escribir_totales(movimientos, n, &respuesta[longitud]));
    return extracto_cerrar(respuesta, longitud);
}
