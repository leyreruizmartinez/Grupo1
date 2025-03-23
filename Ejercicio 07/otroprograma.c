#include <stdio.h>
#include "punto.h"

int main() {
    Punto miPunto = {4, 5};

    printf("Estado inicial:\n");
    imprimirPunto(miPunto);

    escalar(&miPunto, 10);

    printf("Estado despues de escalar:\n");
    imprimirPunto(miPunto);

    return 0;
}