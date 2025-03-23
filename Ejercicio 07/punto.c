#include <stdio.h>
#include "punto.h"

void escalar(Punto *p, int n) {
    p->x += n;
    p->y += n;
}

void imprimirPunto(Punto p) {
    printf("Punto: (%d, %d)\n", p.x, p.y);
}