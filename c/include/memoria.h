#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdint.h>

/*
 * Memoria persistente de la tarjeta. En el equipo de desarrollo es un
 * array en RAM; en la tarjeta sería la EEPROM o la flash, donde una
 * escritura puede fallar: las celdas se gastan y la alimentación se corta.
 */
#define MEMORIA_TAMANO  512u

typedef enum {
    MEMORIA_SIN_ERROR = 0,
    MEMORIA_FALLO_ESCRITURA,
    MEMORIA_DATOS_CORROMPIDOS
} memoria_estado;

/* Deja toda la memoria a cero, como una tarjeta recién fabricada. */
void           memoria_inicializar(void);
uint8_t        memoria_leer(uint16_t direccion);
memoria_estado memoria_escribir(uint16_t direccion, uint8_t valor);

/* El CRC de toda la memoria: cambia en cuanto cambia un byte. */
uint16_t memoria_huella(void);

/* Espera a que el controlador de la memoria termine la escritura en curso. */
void memoria_esperar(void);
/* La rutina que llama el controlador al terminar una escritura. En el equipo de desarrollo la llama la simulación. */
void memoria_interrupcion(void);

/* Solo en el equipo de desarrollo: la siguiente escritura falla, como en una celda gastada. */
void memoria_fallar_siguiente_escritura(void);
/* Solo en el equipo de desarrollo: la siguiente escritura en 'direccion' falla. */
void memoria_fallar_escritura_en(uint16_t direccion);

#endif /* MEMORIA_H */
