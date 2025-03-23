#include <stdio.h>
#include <stdlib.h>

typedef struct {
    double real;
    double imaginario;
} Complex;

void asignarValores(Complex *c, double real, double imaginario) {
    c->real = real;
    c->imaginario = imaginario;
}

void imprimirComplex(const Complex *c) {
    printf("Número complejo: %.2f + %.2fi\n", c->real, c->imaginario);
}

int main() {
    Complex c1;
    asignarValores(&c1, 3.0, 4.0);

    Complex *c2 = (Complex *)malloc(sizeof(Complex));
    if (c2 == NULL) {
        printf("Error al asignar memoria.\n");
        return 1;
    }

    asignarValores(c2, 5.0, -2.0);

    imprimirComplex(&c1);
    imprimirComplex(c2);

    free(c2);

    return 0;
}