#include "unity.h"

#include "comando.h"
#include "memoria.h"
#include "operaciones.h"
#include "pin.h"
#include "registro.h"
#include "saldo.h"

/* La cuenta de pagos y el contador de operaciones, con pagos y recargas de verdad. */
static const uint8_t PIN_TITULAR[] = {1, 2, 3, 4};

static uint16_t enviar_importe(uint8_t ins, uint16_t importe)
{
    uint8_t apdu[7] = {0x80, 0, 0, 0, 2};
    apdu[1] = ins;
    apdu[5] = (uint8_t)(importe >> 8);
    apdu[6] = (uint8_t)(importe & 0xFF);
    return comando_procesar(apdu, sizeof apdu);
}

static void verificar(void)
{
    uint8_t apdu[9] = {0x80, 0x20, 0, 0, 4, 1, 2, 3, 4};
    TEST_ASSERT_EQUAL_HEX16(0x9000, comando_procesar(apdu, sizeof apdu));
}

void setUp(void)
{
    memoria_inicializar();
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, pin_personalizar(PIN_TITULAR, sizeof PIN_TITULAR));
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, saldo_escribir(0u));
}

void tearDown(void) {}

static void test_dos_pagos_dejan_la_cuenta_en_dos(void)
{
    uint8_t total = 0u;

    enviar_importe(0x32, 500);
    verificar();
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar_importe(0x30, 100));
    TEST_ASSERT_EQUAL_HEX16(0x9000, enviar_importe(0x30, 100));
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, registro_contar(&total));
    TEST_ASSERT_EQUAL_UINT8(2, total);
}

static void test_cada_pago_y_cada_recarga_suman_una_operacion(void)
{
    TEST_ASSERT_EQUAL_UINT32(0, operaciones_leer());
    enviar_importe(0x32, 500);
    verificar();
    enviar_importe(0x30, 100);
    TEST_ASSERT_EQUAL_UINT32(2, operaciones_leer());
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_dos_pagos_dejan_la_cuenta_en_dos);
    RUN_TEST(test_cada_pago_y_cada_recarga_suman_una_operacion);
    return UNITY_END();
}
