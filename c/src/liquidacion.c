#include "liquidacion.h"

unsigned short ult_comision;

int liquidar(struct mov *m, int n, long *neto, unsigned short *com, int modo)
{
    int i;
    long tot = 0;
    unsigned short c = 0, x;

    if (n < 0 || n > MAXMOV)
        return -1;

    for (i = 0; i < n; i++) {
        switch (m[i].t) {
        case 1: /* pago */
            tot = tot - m[i].imp;
            /* comision: 1 por ciento, minimo 5 */
            x = m[i].imp / 100;
            if (x < 5)
                x = 5;
            c = c + x;
            break;
        case 2: /* recarga */
            tot = tot + m[i].imp;
            break;
        case 3: /* devolucion */
            tot = tot + m[i].imp;
            /* comision: 1 por ciento, minimo 5 */
            x = m[i].imp / 100;
            if (x < 5)
                x = 5;
            c = c + x;
            break;
        default:
            return -2;
        }
    }

    if (modo == 1) {
        /* redondear la comision a la decena de arriba */
        if (c % 10 != 0)
            c = c + (10 - c % 10);
    }

    *neto = tot;
    *com = c;
    ult_comision = c;
    return 0;
}
