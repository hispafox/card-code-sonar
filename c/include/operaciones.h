#ifndef OPERACIONES_H
#define OPERACIONES_H

#include <stdint.h>

#include "memoria.h"

/* El contador de operaciones (pagos y recargas): cuatro bytes de la memoria persistente, el alto primero. */
uint32_t       operaciones_leer(void);
memoria_estado operaciones_contar(void);

#endif /* OPERACIONES_H */
