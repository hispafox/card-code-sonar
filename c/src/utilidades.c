#include "utilidades.h"

/* En la tarjeta, un fallo así la bloquearía; en el equipo de desarrollo se apunta. */
static uint16_t fallos;
static uint16_t ultimo_fallo_linea;

void util_fallo(const char *fichero, uint16_t linea)
{
    (void)fichero;
    fallos++;
    ultimo_fallo_linea = linea;
}

uint16_t util_fallos(void)
{
    return fallos;
}

uint16_t util_ultimo_fallo_linea(void)
{
    return ultimo_fallo_linea;
}

void util_reiniciar_fallos(void)
{
    fallos = 0u;
    ultimo_fallo_linea = 0u;
}
