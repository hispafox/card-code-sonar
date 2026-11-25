#include "unity.h"

#include "memoria.h"
#include "pin.h"
#include "utilidades.h"

/*
 * Los tres usos que rompían las macros del antes. En el antes no están: con
 * MINIMO, ESCRIBIR_U16 y COMPROBAR, las tres pruebas fallarían.
 */
static const uint8_t PIN_TITULAR[] = {1, 2, 3, 4};

void setUp(void)
{
    memoria_inicializar();
    pin_personalizar(PIN_TITULAR, sizeof PIN_TITULAR);
    pin_modo_personalizacion(0u);
    util_reiniciar_fallos();
}

void tearDown(void) {}

static void test_util_elementos_cuenta_elementos_y_no_bytes(void)
{
    const uint16_t importes[5] = {1, 2, 3, 4, 5};

    TEST_ASSERT_EQUAL_UINT32(5, UTIL_ELEMENTOS(importes));
    TEST_ASSERT_EQUAL_UINT32(10, sizeof importes);
}

static void test_util_minimo_evalua_cada_argumento_una_vez(void)
{
    const uint8_t longitudes[] = {3, 9};
    const uint8_t *p = longitudes;

    TEST_ASSERT_EQUAL_UINT8(3, util_minimo(*p++, 64u));
    TEST_ASSERT_EQUAL_PTR(&longitudes[1], p);    /* ha avanzado una posición, no dos */
}

static void test_util_escribir_u16_en_un_if_sin_llaves_escribe_los_dos_bytes_o_ninguno(void)
{
    uint8_t respuesta[2] = {0xAA, 0xBB};
    uint8_t hay_saldo = 0u;

    if (hay_saldo) util_escribir_u16(respuesta, 0x0123);
    TEST_ASSERT_EQUAL_HEX8(0xAA, respuesta[0]);
    TEST_ASSERT_EQUAL_HEX8(0xBB, respuesta[1]);
}

static void test_fuera_del_modo_de_personalizacion_el_cambio_de_pin_se_rechaza(void)
{
    const uint8_t nuevo[8] = {8, 7, 6, 5, 4, 3, 2, 1};

    TEST_ASSERT_EQUAL_UINT8(0, pin_cambiar(nuevo, sizeof nuevo));
    TEST_ASSERT_EQUAL_UINT16(0, util_fallos());
}

static void test_en_el_modo_de_personalizacion_se_cambia_el_pin(void)
{
    const uint8_t nuevo[8] = {8, 7, 6, 5, 4, 3, 2, 1};

    pin_modo_personalizacion(1u);
    TEST_ASSERT_EQUAL_UINT8(1, pin_cambiar(nuevo, sizeof nuevo));
    TEST_ASSERT_EQUAL_HEX16(PIN_CORRECTO, pin_verificar(nuevo, sizeof nuevo));
}

static void test_en_el_modo_de_personalizacion_un_pin_corto_avisa_del_fallo(void)
{
    pin_modo_personalizacion(1u);
    TEST_ASSERT_EQUAL_UINT8(0, pin_cambiar(PIN_TITULAR, sizeof PIN_TITULAR));
    TEST_ASSERT_EQUAL_UINT16(1, util_fallos());
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_util_elementos_cuenta_elementos_y_no_bytes);
    RUN_TEST(test_util_minimo_evalua_cada_argumento_una_vez);
    RUN_TEST(test_util_escribir_u16_en_un_if_sin_llaves_escribe_los_dos_bytes_o_ninguno);
    RUN_TEST(test_fuera_del_modo_de_personalizacion_el_cambio_de_pin_se_rechaza);
    RUN_TEST(test_en_el_modo_de_personalizacion_se_cambia_el_pin);
    RUN_TEST(test_en_el_modo_de_personalizacion_un_pin_corto_avisa_del_fallo);
    return UNITY_END();
}
