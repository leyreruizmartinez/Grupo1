#include <stdio.h>

int main() {
    char letra;
    
    printf("Introduce letras (presiona 'q' para salir):\n");
    
    while ((letra = getchar()) != 'q') {
        getchar();

        printf("%c\n", letra);

        char mayuscula;
        if (letra >= 'a' && letra <= 'z') {
            mayuscula = letra - 32; 
        } else {
            mayuscula = letra; 
        }

        printf("Has introducido la letra '%c'. ASCII = %d. En macleyuscula: '%c'\n", letra, letra, mayuscula);
    }

    return 0;
}