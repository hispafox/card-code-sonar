#include "unity.h"

#include <stddef.h>

#include "comando.h"
#include "memoria.h"
#include "pin.h"
#include "saldo.h"
#include "seguridad.h"

/*
 * El cambio de PIN deja el modo de personalización cerrado y los dos búferes
 * borrados, salga por donde salga. Los búferes son variables locales: la prueba
 * espía memcpy y seguridad_borrar (opciones en prueba_cambio_pin.opciones), que
 * apuntan qué búferes se copiaron y cuáles se borraron, y después llaman a las de verdad.
 */
void *__real_memcpy(void *destino, const void *origen, size_t n);
void  __real_seguridad_borrar(void *p, size_t n);

static void *copiados[4];
static size_t total_copiados;
static void *borrados[4];
static size_t total_borrados;

void *__wrap_memcpy(void *destino, const void *origen, size_t n)
{
    if (n == PIN_LONGITUD_MAXIMA && total_copiados < 4u) {
        copiados[total_copiados++] = destino;
    }
    return __real_memcpy(destino, origen, n);
}

void __wrap_seguridad_borrar(void *p, size_t n)
{
    if (n == PIN_LONGITUD_MAXIMA && total_borrados < 4u) {
        borrados[total_borrados++] = p;
    }
    __real_seguridad_borrar(p, n);
}

static const uint8_t PIN_ACTUAL[8] = {1, 2, 3, 4, 5, 6, 7, 8};
static const uint8_t PIN_NUEVO[8] = {8, 7, 6, 5, 4, 3, 2, 1};

static uint16_t cambiar_pin(const uint8_t *actual, const uint8_t *nuevo)
{
    uint8_t apdu[5 + 2 * PIN_LONGITUD_MAXIMA] = {0x80, 0x24, 0, 0, 2 * PIN_LONGITUD_MAXIMA};

    __real_memcpy(&apdu[5], actual, PIN_LONGITUD_MAXIMA);
    __real_memcpy(&apdu[5 + PIN_LONGITUD_MAXIMA], nuevo, PIN_LONGITUD_MAXIMA);
    return comando_procesar(apdu, sizeof apdu);
}

void setUp(void)
{
    memoria_inicializar();
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, pin_personalizar(PIN_ACTUAL, sizeof PIN_ACTUAL));
    TEST_ASSERT_EQUAL_INT(MEMORIA_SIN_ERROR, saldo_escribir(0u));
    total_copiados = 0u;
    total_borrados = 0u;
}

void tearDown(void) {}

static uint8_t fue_borrado(const void *bufer)
{
    for (size_t i = 0u; i < total_borrados; i++) {
        if (borrados[i] == bufer) {
            return 1u;
        }
    }
    return 0u;
}

/* Los dos búferes que se copiaron se han borrado (pin_personalizar borra además su copia). */
static void comprobar_que_los_dos_buferes_se_han_borrado(void)
{
    TEST_ASSERT_EQUAL_UINT32(2, total_copiados);
    TEST_ASSERT_TRUE(fue_borrado(copiados[0]));
    TEST_ASSERT_TRUE(fue_borrado(copiados[1]));
}

/* Fuera del modo de personalización, pin_cambiar no cambia nada. */
static void comprobar_que_el_modo_esta_cerrado(void)
{
    TEST_ASSERT_EQUAL_UINT8(0, pin_cambiar(PIN_NUEVO, sizeof PIN_NUEVO));
}

static void test_si_falla_la_escritura_del_pin_nuevo_el_modo_se_cierra_y_los_buferes_se_borran(void)
{
    memoria_fallar_escritura_en(0x0010);    /* la longitud del PIN, lo primero que escribe el cambio */
    TEST_ASSERT_EQUAL_HEX16(0x6581, cambiar_pin(PIN_ACTUAL, PIN_NUEVO));
    comprobar_que_los_dos_buferes_se_han_borrado();
    comprobar_que_el_modo_esta_cerrado();
}

static void test_con_otro_pin_actual_el_modo_sigue_cerrado_y_los_buferes_se_borran(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x6982, cambiar_pin(PIN_NUEVO, PIN_NUEVO));
    comprobar_que_los_dos_buferes_se_han_borrado();
    comprobar_que_el_modo_esta_cerrado();
}

static void test_con_el_pin_actual_el_pin_cambia_y_el_modo_se_cierra(void)
{
    TEST_ASSERT_EQUAL_HEX16(0x9000, cambiar_pin(PIN_ACTUAL, PIN_NUEVO));
    comprobar_que_los_dos_buferes_se_han_borrado();
    comprobar_que_el_modo_esta_cerrado();
    TEST_ASSERT_EQUAL_HEX16(PIN_CORRECTO, pin_verificar(PIN_NUEVO, sizeof PIN_NUEVO));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_si_falla_la_escritura_del_pin_nuevo_el_modo_se_cierra_y_los_buferes_se_borran);
    RUN_TEST(test_con_otro_pin_actual_el_modo_sigue_cerrado_y_los_buferes_se_borran);
    RUN_TEST(test_con_el_pin_actual_el_pin_cambia_y_el_modo_se_cierra);
    return UNITY_END();
}
