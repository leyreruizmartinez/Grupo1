#include "operaciones.h"
#include <stdio.h>

int main() {
    double a = 5.5, b = 2.5;
    int x = 3, y = 4;

    printf("Suma: %.2f\n", sumar(a, b));
    printf("Resta: %.2f\n", restar(a, b));
    printf("Multiplicacion: %d\n", multiplicar(x, y));
    printf("Potencia: %d\n", potencia(x, y));

    return 0;
}