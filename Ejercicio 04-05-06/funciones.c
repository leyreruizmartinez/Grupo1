#include <stdio.h>
#include <math.h>

typedef struct {
    float real;
    float imaginaria;
} Complex;

void imprimir(Complex a) {
    printf("El numero complejo es %.2f + %.2fi\n", a.real, a.imaginaria);
}

Complex sumarComplejos(Complex a, Complex b) {
    Complex resultado;
    resultado.real = a.real + b.real;
    resultado.imaginaria = a.imaginaria + b.imaginaria;
    return resultado;
}

double moduloComplejo(Complex a) {
    return sqrt(a.real * a.real + a.imaginaria * a.imaginaria);
}

int main() {
    Complex num1 = {3.5, 2.5};
    Complex num2 = {1.5, 4.5};

    imprimir(num1);
    imprimir(num2);

    Complex suma = sumarComplejos(num1, num2);
    printf("La suma de los complejos es %.2f + %.2fi\n", suma.real, suma.imaginaria);

    double modulo1 = moduloComplejo(num1);
    double modulo2 = moduloComplejo(num2);

    printf("El modulo de num1 es %.2f\n", modulo1);
    printf("El modulo de num2 es %.2f\n", modulo2);
    printf("El tamanio de la estructura Complex es %lu bytes\n", sizeof(suma));

    return 0;
}