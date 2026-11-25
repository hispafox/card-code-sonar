#include "unity.h"

#include <stdint.h>

#include "comando.h"
#include "memoria.h"
#include "pin.h"
#include "saldo.h"
#include "seguridad.h"

/*
 * Las pruebas nuevas del después. En el portátil no se puede provocar un fallo de
 * hardware: lo que se comprueba es que el código reacciona bien si llega a pasar.
 * pin_verificar se sustituye con el enlazador (prueba_seguridad.opciones) para que
 * el comando reciba un resultado alterado, como si un fallo hubiera cambiado unos bits.
 */
uint16_t __real_pin_verificar(const uint8_t *pin, uint8_t longitud);

static uint8_t  alterar;
static uint16_t resultado_alterado;

uint16_t __wrap_pin_verificar(const uint8_t *pin, uint8_t longitud)
{
    uint16_t resultado = __real_pin_verificar(pin, longitud);

    return alterar ? resultado_alterado : resultado;
}

static const uint8_t PIN_TITULAR[] = {1, 2, 3, 4};

static uint16_t verificar(const uint8_t *pin, uint8_t longitud)
{
    uint8_t apdu[5 + PIN_LONGITUD_MAXIMA] = {0x80, 0x20, 0, 0, 0};

    apdu[4] = longitud;
    for (uint8_t i = 0u; i < longitud; i++) {
        apdu[5 + i] = pin[i];
    }
    return comando_procesar(apdu, (uint8_t)(5 + longitud));
}

void setUp(void)
{
    memoria_inicializar();
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, pin_personalizar(PIN_TITULAR, sizeof PIN_TITULAR));
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, saldo_escribir(0u));
    alterar = 0u;
}

void tearDown(void) {}

static void test_las_dos_constantes_no_comparten_ningun_bit(void)
{
    TEST_ASSERT_EQUAL_HEX16(0xFFFFu, (uint16_t)(PIN_CORRECTO ^ PIN_INCORRECTO));
}

static void test_un_resultado_con_un_bit_cambiado_no_da_acceso(void)
{
    alterar = 1u;
    resultado_alterado = (uint16_t)(PIN_CORRECTO ^ 0x0001u);
    TEST_ASSERT_EQUAL_HEX16(0x6982, verificar(PIN_TITULAR, sizeof PIN_TITULAR));
}

static void test_un_pin_incorrecto_que_un_fallo_cambia_en_unos_bits_no_da_acceso(void)
{
    alterar = 1u;
    resultado_alterado = (uint16_t)(PIN_INCORRECTO ^ 0x00FFu);
    TEST_ASSERT_EQUAL_HEX16(0x6982, verificar(PIN_TITULAR, sizeof PIN_TITULAR));
}

static void test_con_la_tarjeta_bloqueada_el_pin_correcto_ya_no_sirve(void)
{
    tarjeta_bloquear();
    TEST_ASSERT_EQUAL_HEX16(0x6983, verificar(PIN_TITULAR, sizeof PIN_TITULAR));
}

static void test_la_comparacion_da_distinto_de_cero_en_el_primer_byte_y_en_el_ultimo(void)
{
    const uint8_t a[4] = {1, 2, 3, 4};
    const uint8_t primero[4] = {9, 2, 3, 4};
    const uint8_t ultimo[4] = {1, 2, 3, 9};

    TEST_ASSERT_EQUAL_HEX8(0, seguridad_diferencia(a, a, sizeof a));
    TEST_ASSERT_NOT_EQUAL(0, seguridad_diferencia(a, primero, sizeof a));
    TEST_ASSERT_NOT_EQUAL(0, seguridad_diferencia(a, ultimo, sizeof a));
}

static void test_un_pin_con_otra_longitud_es_incorrecto(void)
{
    const uint8_t mas_corto[3] = {1, 2, 3};
    const uint8_t mas_largo[5] = {1, 2, 3, 4, 5};

    TEST_ASSERT_EQUAL_HEX16(PIN_INCORRECTO, pin_verificar(mas_corto, sizeof mas_corto));
    TEST_ASSERT_EQUAL_HEX16(PIN_INCORRECTO, pin_verificar(mas_largo, sizeof mas_largo));
    TEST_ASSERT_EQUAL_HEX16(PIN_CORRECTO, pin_verificar(PIN_TITULAR, sizeof PIN_TITULAR));
}

static void test_el_borrado_seguro_deja_el_bufer_a_cero(void)
{
    uint8_t bufer[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    const uint8_t ceros[8] = {0};

    seguridad_borrar(bufer, sizeof bufer);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(ceros, bufer, sizeof bufer);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_las_dos_constantes_no_comparten_ningun_bit);
    RUN_TEST(test_un_resultado_con_un_bit_cambiado_no_da_acceso);
    RUN_TEST(test_un_pin_incorrecto_que_un_fallo_cambia_en_unos_bits_no_da_acceso);
    RUN_TEST(test_con_la_tarjeta_bloqueada_el_pin_correcto_ya_no_sirve);
    RUN_TEST(test_la_comparacion_da_distinto_de_cero_en_el_primer_byte_y_en_el_ultimo);
    RUN_TEST(test_un_pin_con_otra_longitud_es_incorrecto);
    RUN_TEST(test_el_borrado_seguro_deja_el_bufer_a_cero);
    return UNITY_END();
}
