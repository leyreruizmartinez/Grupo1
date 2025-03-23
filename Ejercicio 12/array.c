#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int numerador;
    int denominador;
} Fraccion;

Fraccion sumar(Fraccion a, Fraccion b) {
    Fraccion resultado;
    resultado.numerador = a.numerador * b.denominador + b.numerador * a.denominador;
    resultado.denominador = a.denominador * b.denominador;
    return resultado;
}

void multiplicar(Fraccion *a, Fraccion *b, Fraccion *resultado) {
    resultado->numerador = a->numerador * b->numerador;
    resultado->denominador = a->denominador * b->denominador;
}

int main() {
    int entero = 10;
    Fraccion frac1;
    int *pEntero;
    Fraccion *pFrac;
    
    pEntero = &entero;
    pFrac = (Fraccion *)malloc(sizeof(Fraccion));
    if (pFrac == NULL) {
        printf("Error al asignar memoria.\n");
        return 1;
    }

    frac1.numerador = 3;
    frac1.denominador = 4;
    pFrac->numerador = 3;
    pFrac->denominador = 4;

    *pEntero = 20;

    Fraccion resultadoSuma = sumar(frac1, *pFrac);
    printf("Suma: %d/%d\n", resultadoSuma.numerador, resultadoSuma.denominador);

    Fraccion resultadoMultiplicacion;
    multiplicar(&frac1, pFrac, &resultadoMultiplicacion);
    printf("Multiplicación: %d/%d\n", resultadoMultiplicacion.numerador, resultadoMultiplicacion.denominador);

    int arrayEnteros[5] = {1, 2, 3, 4, 5};
    Fraccion *arrayFracciones = (Fraccion *)malloc(5 * sizeof(Fraccion));
    if (arrayFracciones == NULL) {
        printf("Error al asignar memoria.\n");
        free(pFrac);
        return 1;
    }

    for (int i = 0; i < 5; i++) {
        arrayFracciones[i].numerador = i + 1;
        arrayFracciones[i].denominador = i + 2;
    }

    arrayEnteros[2] = 100;
    arrayFracciones[2].numerador = 7;
    arrayFracciones[2].denominador = 8;

    int *pArrayEnteros = arrayEnteros;
    Fraccion *pArrayFracciones = arrayFracciones;

    pArrayEnteros[1] = 200;
    pArrayFracciones[1].numerador = 9;
    pArrayFracciones[1].denominador = 10;

    printf("Dirección del último entero: %p\n", (void *)&arrayEnteros[4]);

    int **ppEntero = &pEntero;
    **ppEntero = 30;

    Fraccion **ppFrac = &pFrac;
    (*ppFrac)->numerador = 11;
    (*ppFrac)->denominador = 12;

    free(pFrac);
    free(arrayFracciones);

    return 0;
}
