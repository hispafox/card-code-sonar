#ifndef LIQUIDACION_H
#define LIQUIDACION_H

#define MAXMOV 16

struct mov {
    unsigned char t;      /* 1 pago, 2 recarga, 3 devolucion */
    unsigned short imp;   /* importe en centimos */
};

extern unsigned short ult_comision;

/* Liquida los movimientos del dia. modo 1 = redondear la comision.
   Devuelve 0 si va bien, -1 si n no vale, -2 si hay un tipo que no existe. */
int liquidar(struct mov *m, int n, long *neto, unsigned short *com, int modo);

#endif
