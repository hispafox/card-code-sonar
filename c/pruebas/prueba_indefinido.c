#include "unity.h"

#include <stddef.h>
#include <stdint.h>

#include "ejemplos.h"
#include "memoria.h"
#include "operaciones.h"
#include "pin.h"
#include "registro.h"
#include "saldo.h"

/*
 * Las pruebas nuevas del después, compiladas con el detector de comportamiento
 * indefinido en modo trampa (prueba_indefinido.opciones): si alguna llega a un
 * caso indefinido, el programa se para en esa instrucción. Con el código del
 * antes, la de 0x80000001 se pararía en util_leer_u32, la de la cuenta sin pagos
 * devolvería lo que hubiera en la pila, y las de los ejemplos no pasarían.
 */
static const uint8_t PIN_TITULAR[] = {1, 2, 3, 4};

void setUp(void)
{
    memoria_inicializar();
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, pin_personalizar(PIN_TITULAR, sizeof PIN_TITULAR));
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, saldo_escribir(0u));
}

void tearDown(void) {}

static void test_contar_los_pagos_de_una_tarjeta_sin_ninguno_dice_que_no_hay_cuenta(void)
{
    uint8_t total = 0u;

    TEST_ASSERT_EQUAL_INT(MEMORIA_DATOS_CORROMPIDOS, registro_contar(&total));
}

static void test_un_contador_con_el_byte_alto_a_0x80_se_lee_entero(void)
{
    const uint8_t bytes[4] = {0x80, 0x00, 0x00, 0x01};

    for (uint16_t i = 0u; i < 4u; i++) {
        memoria_escribir((uint16_t)(0x0090u + i), bytes[i]);
    }
    TEST_ASSERT_EQUAL_HEX32(0x80000001u, operaciones_leer());
}

static void test_el_contador_al_maximo_da_la_vuelta_a_cero(void)
{
    for (uint16_t i = 0u; i < 4u; i++) {
        memoria_escribir((uint16_t)(0x0090u + i), 0xFF);
    }
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, operaciones_contar());
    TEST_ASSERT_EQUAL_HEX32(0u, operaciones_leer());
}

static void test_sumar_cien_cerca_del_maximo_devuelve_el_error(void)
{
    TEST_ASSERT_EQUAL_INT32(-1, sumar_cien(INT32_MAX));
    TEST_ASSERT_EQUAL_INT32(-1, sumar_cien(INT32_MAX - 99));
    TEST_ASSERT_EQUAL_INT32(INT32_MAX, sumar_cien(INT32_MAX - 100));
    TEST_ASSERT_EQUAL_INT32(105, sumar_cien(5));
}

static void test_leer_primero_con_un_puntero_nulo_devuelve_cero(void)
{
    const int32_t valor = 42;

    TEST_ASSERT_EQUAL_INT32(0, leer_primero(NULL));
    TEST_ASSERT_EQUAL_INT32(42, leer_primero(&valor));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_contar_los_pagos_de_una_tarjeta_sin_ninguno_dice_que_no_hay_cuenta);
    RUN_TEST(test_un_contador_con_el_byte_alto_a_0x80_se_lee_entero);
    RUN_TEST(test_el_contador_al_maximo_da_la_vuelta_a_cero);
    RUN_TEST(test_sumar_cien_cerca_del_maximo_devuelve_el_error);
    RUN_TEST(test_leer_primero_con_un_puntero_nulo_devuelve_cero);
    return UNITY_END();
}
