#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registro.h"
#include "bd.h"

int main(void) {
    inicializarCSV();
    int opcion;
    do {
        printf("\n------ 📖 PÁGINA DE REGISTRO E INICIO DE SESIÓN 📖 -------\n");
        printf("\t1. Registrar usuario\n");
        printf("\t2. Mostrar usuarios\n");
        printf("\t3. Iniciar sesión\n");
        printf("\t4. Salir\n");
        printf("\nSeleccione una opción: ");
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
                printf("Inicio de sesión\n");
                iniciarSesion();
                printf("\n------------------------------------------------------------------------------------\n");
                break;
            case 4:
                printf("\nSaliendo... 👋\n");
                break;
            default:
                printf("\nOpción inválida. Intente de nuevo.\n");
        }
    } while (opcion != 4);
    return 0;
}