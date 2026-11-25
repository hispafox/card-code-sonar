#include "pin.h"

#include "memoria.h"
#include "seguridad.h"
#include "utilidades.h"

#define PIN_DIRECCION_LONGITUD  0x0010u
#define PIN_DIRECCION_DIGITOS   0x0011u
#define PIN_DIRECCION_INTENTOS  0x0020u

static uint8_t pin_intentos = PIN_INTENTOS_MAXIMOS;
static uint8_t pin_comprobado;
static uint8_t modo_personalizacion;
static uint8_t pin_rechazado;

/* Los intentos se guardan en la memoria persistente: un corte de alimentación no los devuelve. */
static memoria_estado pin_guardar_intentos(void)
{
    memoria_estado estado;

    estado = memoria_escribir(PIN_DIRECCION_INTENTOS, pin_intentos);
    return estado;
}

static void pin_rechazar(void)
{
    pin_rechazado = 1u;
}

static void pin_copiar_guardado(uint8_t *destino, size_t n)
{
    for (size_t i = 0u; i < n; i++) {
        destino[i] = memoria_leer((uint16_t)(PIN_DIRECCION_DIGITOS + i));
    }
}

memoria_estado pin_personalizar(const uint8_t *pin, uint8_t longitud)
{
    uint8_t        pin_guardado[PIN_LONGITUD_MAXIMA];
    memoria_estado estado = memoria_escribir(PIN_DIRECCION_LONGITUD, longitud);

    for (uint8_t i = 0u; i < longitud && estado == MEMORIA_SIN_ERROR; i++) {
        estado = memoria_escribir((uint16_t)(PIN_DIRECCION_DIGITOS + i), pin[i]);
    }
    if (estado == MEMORIA_SIN_ERROR) {
        /* Lo escrito en la memoria persistente se relee para comprobarlo. */
        pin_copiar_guardado(pin_guardado, sizeof pin_guardado);
        for (uint8_t i = 0u; i < longitud; i++) {
            if (pin_guardado[i] != pin[i]) {
                estado = MEMORIA_FALLO_ESCRITURA;
            }
        }
        seguridad_borrar(pin_guardado, sizeof pin_guardado);
    }
    pin_intentos = PIN_INTENTOS_MAXIMOS;
    pin_comprobado = 0u;
    if (estado == MEMORIA_SIN_ERROR) {
        estado = pin_guardar_intentos();
    }
    return estado;
}

uint8_t pin_personalizado(void)
{
    return (uint8_t)(memoria_leer(PIN_DIRECCION_LONGITUD) != 0u);
}

/*
 * CONTRAMEDIDA (canal lateral): cero solo si el PIN y su longitud coinciden con
 * los guardados. La longitud entra en la cuenta en lugar de salir antes, y los
 * dígitos se comparan todos; el tiempo solo depende de la longitud que manda el terminal.
 */
static uint8_t pin_diferencia(const uint8_t *pin, uint8_t longitud)
{
    uint8_t pin_guardado[PIN_LONGITUD_MAXIMA];
    uint8_t diferencia = (uint8_t)(longitud ^ memoria_leer(PIN_DIRECCION_LONGITUD));

    if (longitud > PIN_LONGITUD_MAXIMA) {
        return 0xFFu;                       /* no cabe en el búfer: no puede ser el PIN */
    }
    pin_copiar_guardado(pin_guardado, sizeof pin_guardado);
    diferencia |= seguridad_diferencia(pin, pin_guardado, longitud);
    seguridad_borrar(pin_guardado, sizeof pin_guardado);
    return diferencia;
}

uint16_t pin_verificar(const uint8_t *pin, uint8_t longitud)
{
    uint16_t resultado = PIN_INCORRECTO;

    pin_comprobado = 0u;
    if (pin_intentos == 0u) {
        return PIN_INCORRECTO;
    }
    pin_intentos--;    /* el intento se gasta antes de comparar */
    if (pin_guardar_intentos() != MEMORIA_SIN_ERROR) {
        return PIN_INCORRECTO;
    }
    if (pin_diferencia(pin, longitud) == 0u) {
        pin_intentos = PIN_INTENTOS_MAXIMOS;
        if (pin_guardar_intentos() == MEMORIA_SIN_ERROR) {
            resultado = PIN_CORRECTO;
            pin_comprobado = 1u;
        }
    }
    return resultado;
}

uint8_t pin_intentos_restantes(void)
{
    return pin_intentos;
}

void pin_bloquear(void)
{
    pin_intentos = 0u;
    pin_comprobado = 0u;
    (void)pin_guardar_intentos();    /* con la memoria fallando, el bloqueo en RAM ya vale hasta el siguiente arranque */
}

uint8_t pin_verificado(void)
{
    return pin_comprobado;
}

void pin_modo_personalizacion(uint8_t activo)
{
    modo_personalizacion = activo;
}

uint8_t pin_cambiar(const uint8_t *pin, uint8_t longitud)
{
    pin_rechazado = 0u;
    if (modo_personalizacion)
        UTIL_COMPROBAR(longitud == PIN_LONGITUD_MAXIMA);
    else
        pin_rechazar();
    if (pin_rechazado || longitud != PIN_LONGITUD_MAXIMA) {
        return 0u;
    }
    return (uint8_t)(pin_personalizar(pin, longitud) == MEMORIA_SIN_ERROR);
}
