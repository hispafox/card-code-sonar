#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <stdint.h>

/* Que el compilador avise si alguien descarta lo que devuelve la función. Es propio de GCC. */
#if defined(__GNUC__)
#define UTIL_NO_IGNORAR __attribute__((warn_unused_result))
#else
#define UTIL_NO_IGNORAR
#endif

/* Cuántos elementos tiene un array. Solo vale con el array, no con un puntero a él. */
#define UTIL_ELEMENTOS(a)  (sizeof (a) / sizeof (a)[0])

static inline uint8_t util_minimo(uint8_t a, uint8_t b)
{
    return (a < b) ? a : b;
}

static inline uint16_t util_leer_u16(const uint8_t *p)
{
    return (uint16_t)(((uint16_t)p[0] << 8) | p[1]);
}

static inline void util_escribir_u16(uint8_t *p, uint16_t valor)
{
    p[0] = (uint8_t)(valor >> 8);
    p[1] = (uint8_t)valor;
}

static inline uint32_t util_leer_u32(const uint8_t *p)
{
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
}

static inline void util_escribir_u32(uint8_t *p, uint32_t valor)
{
    p[0] = (uint8_t)(valor >> 24);
    p[1] = (uint8_t)(valor >> 16);
    p[2] = (uint8_t)(valor >> 8);
    p[3] = (uint8_t)valor;
}

/* Avisa de que una comprobación no se ha cumplido, con el fichero y la línea. */
void util_fallo(const char *fichero, uint16_t linea);

#define UTIL_COMPROBAR(c)                     \
    do {                                      \
        if (!(c)) {                           \
            util_fallo(__FILE__, __LINE__);   \
        }                                     \
    } while (0)

/* Solo en el equipo de desarrollo: cuántos fallos se han avisado y el último. */
uint16_t util_fallos(void);
uint16_t util_ultimo_fallo_linea(void);
void     util_reiniciar_fallos(void);

#endif /* UTILIDADES_H */
