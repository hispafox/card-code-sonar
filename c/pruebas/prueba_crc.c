#include "unity.h"

#include <stddef.h>
#include <stdint.h>
#include "crc.h"

/* El CRC-16 con valores conocidos: la versión con tabla da lo mismo que la de ocho vueltas por byte. */
void setUp(void) {}
void tearDown(void) {}

static void test_el_valor_de_comprobacion_conocido(void)
{
    const uint8_t datos[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    TEST_ASSERT_EQUAL_HEX16(0x29B1u, crc_calcular(datos, sizeof datos));
}

static void test_sin_datos_queda_el_valor_inicial(void)
{
    TEST_ASSERT_EQUAL_HEX16(0xFFFFu, crc_calcular(NULL, 0));
}

static void test_un_bloque_con_todos_los_valores_de_un_byte(void)
{
    uint8_t datos[256];
    for (size_t i = 0; i < sizeof datos; i++) {
        datos[i] = (uint8_t)i;
    }
    TEST_ASSERT_EQUAL_HEX16(0x3FBDu, crc_calcular(datos, sizeof datos));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_el_valor_de_comprobacion_conocido);
    RUN_TEST(test_sin_datos_queda_el_valor_inicial);
    RUN_TEST(test_un_bloque_con_todos_los_valores_de_un_byte);
    return UNITY_END();
}
