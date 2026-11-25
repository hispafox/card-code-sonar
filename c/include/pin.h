#ifndef PIN_H
#define PIN_H

#include <stdint.h>

#include "memoria.h"

/* Una compilación de prueba puede cambiarla con -DPIN_LONGITUD_MAXIMA=12u. */
#ifndef PIN_LONGITUD_MAXIMA
#define PIN_LONGITUD_MAXIMA 8u
#endif
#define PIN_INTENTOS_MAXIMOS 3u

/* Los dos resultados de la verificación, complementarios: no comparten ningún bit. */
#define PIN_CORRECTO    0xA5C3u
#define PIN_INCORRECTO  0x5A3Cu

/*
 * Graba el PIN del titular en la memoria persistente, lo relee para comprobar
 * que ha quedado bien y deja los intentos al máximo.
 */
memoria_estado pin_personalizar(const uint8_t *pin, uint8_t longitud);
/* 1 si la tarjeta ya tiene un PIN grabado. */
uint8_t        pin_personalizado(void);

uint16_t pin_verificar(const uint8_t *pin, uint8_t longitud);   /* PIN_CORRECTO o PIN_INCORRECTO */
uint8_t pin_intentos_restantes(void);
/* Deja el PIN sin intentos, también en la memoria persistente. */
void    pin_bloquear(void);
uint8_t pin_verificado(void);

/*
 * Cambia el PIN. Solo se admite en el modo de personalización, y entonces el
 * PIN nuevo tiene que tener la longitud máxima. Devuelve 1 si lo ha cambiado,
 * y 0 si no se admite o si ha fallado la escritura.
 */
void    pin_modo_personalizacion(uint8_t activo);
uint8_t pin_cambiar(const uint8_t *pin, uint8_t longitud);

#endif /* PIN_H */
