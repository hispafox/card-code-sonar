#include "unity.h"

#include <stdint.h>

#include "extracto.h"

static const extracto_movimiento CUATRO[] = {
    {EXTRACTO_PAGO, 1000u}, {EXTRACTO_RECARGA, 2000u}, {EXTRACTO_PAGO, 50u}, {EXTRACTO_DEVOLUCION, 300u},
};

static uint8_t respuesta[64];

void setUp(void) {}
void tearDown(void) {}

static void test_sin_movimientos_solo_la_cabecera_y_la_suma(void)
{
    const uint8_t esperado[] = {0x01, 0x00, 0x01};

    TEST_ASSERT_EQUAL_UINT16(sizeof esperado, extracto_generar(CUATRO, 0u, respuesta, sizeof respuesta));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(esperado, respuesta, sizeof esperado);
}

static void test_un_pago(void)
{
    const extracto_movimiento pago[] = {{EXTRACTO_PAGO, 250u}};
    const uint8_t esperado[] = {0x01, 0x01, 'P', 0x00, 0xFA, 0xAA};

    TEST_ASSERT_EQUAL_UINT16(sizeof esperado, extracto_generar(pago, 1u, respuesta, sizeof respuesta));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(esperado, respuesta, sizeof esperado);
}

static void test_cuatro_movimientos(void)
{
    const uint8_t esperado[] = {0x01, 0x04, 'P', 0x03, 0xE8, 'R', 0x07, 0xD0, 'P', 0x00, 0x32,
                                'D', 0x01, 0x2C, 0x30};

    TEST_ASSERT_EQUAL_UINT16(sizeof esperado, extracto_generar(CUATRO, 4u, respuesta, sizeof respuesta));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(esperado, respuesta, sizeof esperado);
}

static void test_cuatro_movimientos_con_totales(void)
{
    const uint8_t esperado[] = {0x01, 0x04, 'P', 0x03, 0xE8, 'R', 0x07, 0xD0, 'P', 0x00, 0x32,
                                'D', 0x01, 0x2C, 0x04, 0x1A, 0x07, 0xD0, 0x01, 0x2C, 0xD4};

    TEST_ASSERT_EQUAL_UINT16(sizeof esperado, extracto_generar_con_totales(CUATRO, 4u, respuesta, sizeof respuesta));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(esperado, respuesta, sizeof esperado);
}

static void test_sin_movimientos_con_totales_a_cero(void)
{
    const uint8_t esperado[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

    TEST_ASSERT_EQUAL_UINT16(sizeof esperado, extracto_generar_con_totales(CUATRO, 0u, respuesta, sizeof respuesta));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(esperado, respuesta, sizeof esperado);
}

static void test_un_tipo_desconocido_no_genera_nada(void)
{
    const extracto_movimiento cero[] = {{EXTRACTO_PAGO, 10u}, {0u, 10u}};
    const extracto_movimiento cuatro[] = {{EXTRACTO_PAGO, 10u}, {4u, 10u}};

    TEST_ASSERT_EQUAL_UINT16(0u, extracto_generar(cero, 2u, respuesta, sizeof respuesta));
    TEST_ASSERT_EQUAL_UINT16(0u, extracto_generar_con_totales(cuatro, 2u, respuesta, sizeof respuesta));
}

static void test_cabe_justo_y_un_byte_menos_no(void)
{
    TEST_ASSERT_EQUAL_UINT16(15u, extracto_generar(CUATRO, 4u, respuesta, 15u));
    TEST_ASSERT_EQUAL_UINT16(0u, extracto_generar(CUATRO, 4u, respuesta, 14u));
}

static void test_con_totales_cabe_justo_y_un_byte_menos_no(void)
{
    TEST_ASSERT_EQUAL_UINT16(21u, extracto_generar_con_totales(CUATRO, 4u, respuesta, 21u));
    TEST_ASSERT_EQUAL_UINT16(0u, extracto_generar_con_totales(CUATRO, 4u, respuesta, 20u));
}

static void test_mas_de_ocho_movimientos_no_genera_nada(void)
{
    extracto_movimiento nueve[9];

    for (uint8_t i = 0u; i < 9u; i++) {
        nueve[i].tipo = EXTRACTO_PAGO;
        nueve[i].importe = 1u;
    }
    TEST_ASSERT_EQUAL_UINT16(0u, extracto_generar(nueve, 9u, respuesta, sizeof respuesta));
    TEST_ASSERT_EQUAL_UINT16(0u, extracto_generar_con_totales(nueve, 9u, respuesta, sizeof respuesta));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_sin_movimientos_solo_la_cabecera_y_la_suma);
    RUN_TEST(test_un_pago);
    RUN_TEST(test_cuatro_movimientos);
    RUN_TEST(test_cuatro_movimientos_con_totales);
    RUN_TEST(test_sin_movimientos_con_totales_a_cero);
    RUN_TEST(test_un_tipo_desconocido_no_genera_nada);
    RUN_TEST(test_cabe_justo_y_un_byte_menos_no);
    RUN_TEST(test_con_totales_cabe_justo_y_un_byte_menos_no);
    RUN_TEST(test_mas_de_ocho_movimientos_no_genera_nada);
    return UNITY_END();
}
