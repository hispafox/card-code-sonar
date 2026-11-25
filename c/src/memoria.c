#include "memoria.h"

#include <string.h>

#include "crc.h"

#define MEMORIA_ESTADO_OCUPADA  0x01u
#define MEMORIA_ESTADO_ERROR    0x02u
#define MEMORIA_SIN_DIRECCION   0xFFFFu

static uint8_t celdas[MEMORIA_TAMANO];
static uint8_t fallar_siguiente;
static uint16_t fallar_en = MEMORIA_SIN_DIRECCION;

/* Lo que en la tarjeta son el registro de estado del controlador y el indicador que pone su interrupción. */
static volatile uint8_t registro_estado_memoria;
static volatile uint8_t escritura_terminada;   /* lo pone a 1 la interrupción de la memoria */

void memoria_inicializar(void)
{
    memset(celdas, 0, sizeof celdas);
    fallar_siguiente = 0u;
    fallar_en = MEMORIA_SIN_DIRECCION;
    registro_estado_memoria = 0u;
}

uint8_t memoria_leer(uint16_t direccion)
{
    if (direccion >= MEMORIA_TAMANO) {
        return 0u;
    }
    return celdas[direccion];
}

void memoria_interrupcion(void)
{
    escritura_terminada = 1u;
}

void memoria_esperar(void)
{
    while (!escritura_terminada) {
    }
}

/* La simulación del controlador: escribe la celda, o falla si las pruebas lo han pedido, y avisa al terminar. */
static void memoria_controlador_escribir(uint16_t direccion, uint8_t valor)
{
    if (fallar_siguiente || direccion == fallar_en) {
        fallar_siguiente = 0u;
        fallar_en = MEMORIA_SIN_DIRECCION;
        registro_estado_memoria = MEMORIA_ESTADO_ERROR;
    } else {
        celdas[direccion] = valor;
        registro_estado_memoria = 0u;
    }
    memoria_interrupcion();
}

memoria_estado memoria_escribir(uint16_t direccion, uint8_t valor)
{
    uint8_t estado;

    if (direccion >= MEMORIA_TAMANO) {
        return MEMORIA_FALLO_ESCRITURA;
    }
    escritura_terminada = 0u;
    registro_estado_memoria = MEMORIA_ESTADO_OCUPADA;
    memoria_controlador_escribir(direccion, valor);
    memoria_esperar();
    estado = registro_estado_memoria;   /* una lectura del registro volatile */

    if (estado & MEMORIA_ESTADO_ERROR) {
        return MEMORIA_FALLO_ESCRITURA;
    }
    if (estado & MEMORIA_ESTADO_OCUPADA) {
        return MEMORIA_FALLO_ESCRITURA;      /* terminada y ocupada a la vez: no se fía */
    }
    return MEMORIA_SIN_ERROR;
}

void memoria_fallar_siguiente_escritura(void)
{
    fallar_siguiente = 1u;
}

void memoria_fallar_escritura_en(uint16_t direccion)
{
    fallar_en = direccion;
}

uint16_t memoria_huella(void)
{
    const uint8_t *bufer = celdas;
    size_t longitud = MEMORIA_TAMANO;
    uint16_t crc;

    crc = crc_calcular(bufer, longitud);
    return crc;
}
