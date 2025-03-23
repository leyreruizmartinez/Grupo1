#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Implementar un módulo en C y probarlo, que permita:
    // - Probar las funciones con un programa principal
    // - Cuidado con el '\0' que indica el final de las cadenas de caracteres

 // - Imprimir un array de caracteres elemento a elemento
char imprimirArray(char array[]) {
    int i = 0;
    while (array[i] != '\0') {
        printf("%c", array[i]);
        i++;
    }
    return 0;
}

int main(void) {
    char * listaChar = (char *) malloc(6 * sizeof(char));
    listaChar[0] = 'b';
    listaChar[1] = 'r';
    listaChar[2] = 'i';
    listaChar[3] = 'a';
    listaChar[4] = 'r';
    listaChar[5] = '\0';

    // - Obtener el tamaño de la cadena de caracteres
    int size = strlen(listaChar);

    printf("Array de caracteres: ");
    imprimirArray(listaChar);
    printf("\n\n");

    printf("Tamaño de la cadena: %d\n\n", size);

    // - Clonar una cadena de caracteres
    char * listaCharClon = (char *) malloc(6 * sizeof(char));
    strcpy(listaCharClon, listaChar);

    printf("Array de caracteres clonado: ");
    imprimirArray(listaCharClon);
    printf("\n\n");

    // - Concatenar una cadena de caracteres a otra
    char * listaCharConct = (char * ) malloc(11 * sizeof(char));
    listaCharConct = strcat(listaCharClon, listaChar);
    printf("Array de caracteres concatenado: ");
    imprimirArray(listaCharConct);
    printf("\n\n");

     // - Copiar una cadena de caracteres en otra
     char * listaCharCop = (char *) malloc(6 * sizeof(char));
     listaCharCop[0] = 'j';
     listaCharCop[1] = 'a';
     listaCharCop[2] = 'y';
     listaCharCop[3] = 'c';
     listaCharCop[4] = 'e';
     listaCharCop[5] = '\0';

    listaCharCop = strcpy(listaChar, listaCharCop);

    printf("Array de caracteres copiada: ");
    imprimirArray(listaCharCop);
    printf("\n\n");
}