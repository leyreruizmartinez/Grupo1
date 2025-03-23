#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int x;
    int y;
} Point;

void escalar(Point **puntos, int *escalas, int tam) {
    for (int i = 0; i < tam; i++) {
        puntos[i]->x *= escalas[i];
        puntos[i]->y *= escalas[i];
    }
}

void imprimirPuntos(Point **puntos, int tam) {
    for (int i = 0; i < tam; i++) {
        printf("Punto %d: (%d, %d)\n", i + 1, puntos[i]->x, puntos[i]->y);
    }
}

int main() {
    int escalares[3] = {2, 3, 4};

    Point *puntos[3];
    for (int i = 0; i < 3; i++) {
        puntos[i] = (Point *)malloc(sizeof(Point));
        if (puntos[i] == NULL) {
            printf("Error al asignar memoria.\n");
            return 1;
        }
    }

    puntos[0]->x = 1; puntos[0]->y = 2;
    puntos[1]->x = 3; puntos[1]->y = 4;
    puntos[2]->x = 5; puntos[2]->y = 6;

    escalar(puntos, escalares, 3);
    imprimirPuntos(puntos, 3);

    for (int i = 0; i < 3; i++) {
        free(puntos[i]);
    }

    return 0;
}