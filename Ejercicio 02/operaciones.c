#include "operaciones.h"

double sumar(double a, double b) {
    return a + b;
}

double restar(double a, double b) {
    return a - b;
}

int multiplicar(int a, int b) {
    int resultado = 0;
    for (int i = 0; i < b; i++) {
        resultado += a;
    }
    return resultado;
}

int potencia(int base, int exponente) {
    int resultado = 1;
    for (int i = 0; i < exponente; i++) {
        resultado *= base;
    }
    return resultado;
}