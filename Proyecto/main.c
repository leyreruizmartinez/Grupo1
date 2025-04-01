#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registro.h"
#include "bd.h"

int main(void) {
    inicializarCSV();
    int opcion;
    do {
        printf("\n------ PAGINA DE REGISTRO E INICIO DE SESION -------\n");
        printf("\t1. Registrar usuario\n");
        printf("\t2. Mostrar usuarios\n");
        printf("\t3. Iniciar sesion\n");
        printf("\t4. Salir\n");
        printf("\nSeleccione una opcion: ");
        scanf("%d", &opcion);
        getchar();

        printf("\n------------------------------------------------------------------------------------\n");
        switch (opcion) {
            case 1:
                printf("Registro\n");
                registrarUsuario();
                printf("\n------------------------------------------------------------------------------------\n");
                break;
            case 2:
                mostrarUsuarios();
                printf("\n------------------------------------------------------------------------------------\n");
                break;
            case 3:
                printf("Inicio de sesion\n");
                iniciarSesion();
                printf("\n------------------------------------------------------------------------------------\n");
                break;
            case 4:
                printf("\nSaliendo...\n");
                break;
            default:
                printf("\nOpción invalida. Intente de nuevo.\n");
        }
    } while (opcion != 4);
    return 0;
}