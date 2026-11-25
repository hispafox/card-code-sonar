#ifndef COMANDO_H
#define COMANDO_H

#include <stdint.h>

#define COMANDO_LONGITUD_MAXIMA  64u

/* Procesa un comando completo («longitud» cuenta la cabecera y los datos) y devuelve su código de estado. */
uint16_t comando_procesar(const uint8_t *apdu, uint8_t longitud);

/* La respuesta del último comando: los datos, si los hay, y el código de estado. */
const uint8_t *comando_ultima_respuesta(uint16_t *longitud);

/*
 * Las respuestas son un conjunto fijo, reservado al compilar. comando_procesar
 * toma una para cada comando y suelta la del anterior; quien necesite otra a la
 * vez la toma aquí. Devuelve NULL si están todas en uso.
 */
typedef struct comando_respuesta comando_respuesta;

comando_respuesta *comando_respuesta_tomar(void);
void               comando_respuesta_soltar(const comando_respuesta *r);

#endif /* COMANDO_H */
