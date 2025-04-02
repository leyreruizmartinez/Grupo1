#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "registro.h"
#include "bd.h"

#define DB_NAME "libros.db"

int main(void) {
    sqlite3* db = NULL; 
    // Abrir la base de datos
    if (sqlite3_open(DB_NAME, &db)) {
        fprintf(stderr, "Error al abrir la base de datos: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Inicializar la base de datos y cargar datos desde CSV
    inicializarBaseDeDatos(db);
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
        getchar(); // Limpiar el buffer de entrada

        printf("\n------------------------------------------------------------------------------------\n");
        switch (opcion) {
            case 1:
                printf("Registro\n");
                registrarUsuario();
                printf("\n------------------------------------------------------------------------------------\n");
                break;
            case 2:
                printf("Mostrar usuarios\n");
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

    // Cargar datos desde archivos CSV
    cargarLibrosDesdeCSV(db, "libros.csv");
    cargarUsuariosDesdeCSV(db, "usuarios.csv");
    cargarHistorialDesdeCSV(db, "historial.csv");
    cargarPrestamosDesdeCSV(db, "prestamos.csv");

    // Cerrar la base de datos
    sqlite3_close(db);
    return 0;
}