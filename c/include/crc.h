#ifndef CRC_H
#define CRC_H

#include <stddef.h>
#include <stdint.h>

/* Calcula el CRC-16 de 'longitud' bytes de 'datos'. */
uint16_t crc_calcular(const uint8_t *datos, size_t longitud);

#endif /* CRC_H */
