#include <stdio.h>

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

void restarComplejos(Complex *c1, Complex c2) {
    c1->real -= c2.real;
    c1->imaginaria -= c2.imaginaria;
}

int main() {
    Complex num1 = {3.5, 2.5};
    Complex num2 = {1.5, 4.5};

    imprimir(num1);
    imprimir(num2);

    Complex suma = sumarComplejos(num1, num2);
    printf("La suma de los complejos es %.2f + %.2fi\n", suma.real, suma.imaginaria);

    restarComplejos(&num1, num2);
    printf("La resta de los complejos es %.2f + %.2fi\n", num1.real, num1.imaginaria);

    return 0;
}