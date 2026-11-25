#include "unity.h"

#include <stddef.h>
#include <string.h>

#include "comando.h"
#include "memoria.h"
#include "pin.h"
#include "saldo.h"
#include "seguridad.h"

/*
 * Con el PIN a doce bytes (esta prueba se compila con -DPIN_LONGITUD_MAXIMA=12u,
 * en prueba_pin_doce.opciones), los borrados y la copia del PIN guardado siguen
 * el tamaño sin tocar el código. La prueba espía seguridad_borrar y apunta
 * cuántos bytes pone a cero cada llamada.
 */
void __real_seguridad_borrar(void *p, size_t n);

static size_t borrados[8];
static size_t total_borrados;

void __wrap_seguridad_borrar(void *p, size_t n)
{
    if (total_borrados < 8u) {
        borrados[total_borrados++] = n;
    }
    __real_seguridad_borrar(p, n);
}

static const uint8_t PIN_DOCE[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3};

void setUp(void)
{
    memoria_inicializar();
    total_borrados = 0u;
}

void tearDown(void) {}

static void test_la_longitud_maxima_es_doce_en_esta_compilacion(void)
{
    TEST_ASSERT_EQUAL_UINT32(12, PIN_LONGITUD_MAXIMA);
}

static void test_al_personalizar_la_copia_de_comprobacion_se_borra_entera(void)
{
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, pin_personalizar(PIN_DOCE, sizeof PIN_DOCE));
    TEST_ASSERT_EQUAL_UINT32(1, total_borrados);
    TEST_ASSERT_EQUAL_UINT32(12, borrados[0]);
}

static void test_al_verificar_el_pin_recibido_se_borra_entero(void)
{
    uint8_t apdu[5 + 12] = {0x80, 0x20, 0, 0, 12};

    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, pin_personalizar(PIN_DOCE, sizeof PIN_DOCE));
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, saldo_escribir(0u));
    memcpy(&apdu[5], PIN_DOCE, sizeof PIN_DOCE);
    total_borrados = 0u;
    TEST_ASSERT_EQUAL_HEX16(0x9000, comando_procesar(apdu, sizeof apdu));
    TEST_ASSERT_EQUAL_UINT32(2, total_borrados);    /* la copia del PIN guardado y el PIN recibido */
    TEST_ASSERT_EQUAL_UINT32(12, borrados[0]);
    TEST_ASSERT_EQUAL_UINT32(12, borrados[1]);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_la_longitud_maxima_es_doce_en_esta_compilacion);
    RUN_TEST(test_al_personalizar_la_copia_de_comprobacion_se_borra_entera);
    RUN_TEST(test_al_verificar_el_pin_recibido_se_borra_entero);
    return UNITY_END();
}
