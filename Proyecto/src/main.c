#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bd/sqlite3.h"
#include "registro.h"
#include "bd.h"
#include "log.h"

#define DB_NAME "datos/libros.db"

int main(void) {
    const char *nombreArchivo = DB_NAME;
    sqlite3* db = NULL;

    if (sqlite3_open(DB_NAME, &db)) {
        fprintf(stderr, "Error al abrir la base de datos: %s\n", sqlite3_errmsg(db));
        registrarLog("Error al abrir la base de datos en main.");
        return 1;
    }
    registrarLog("Base de datos abierta correctamente en main.");

    sqlite3_close_v2(db);

    if (remove(nombreArchivo) == 0) {
        printf("Rehaciendo base de datos...\n");
        registrarLog("Base de datos eliminada correctamente.");
    } else {
        printf("No se pudo eliminar.\n");
        registrarLog("No se pudo eliminar la base de datos.");
    }

    inicializarBaseDeDatos(db);
    registrarLog("Base de datos inicializada.");

    inicializarCSV();
    registrarLog("CSV inicializados.");

    cargarLibrosDesdeCSV(db, "datos/libros.csv");
    registrarLog("Libros cargados desde CSV.");

    cargarUsuariosDesdeCSV(db, "datos/usuarios.csv");
    registrarLog("Usuarios cargados desde CSV.");

    cargarHistorialDesdeCSV(db, "datos/historial.csv");
    registrarLog("Historial cargado desde CSV.");

    cargarPrestamosDesdeCSV(db, "datos/prestamos.csv");
    registrarLog("Préstamos cargados desde CSV.");

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
                registrarLog("Registro de nuevo usuario.");
                registrarUsuario(db);
                break;
            case 2:
                registrarLog("Listado de usuarios mostrado.");
                mostrarUsuarios(db);
                break;
            case 3:
                registrarLog("Inicio de sesión de usuario.");
                iniciarSesion(db);
                break;
            case 4:
                registrarLog("Aplicación finalizada por el usuario.");
                printf("\nSaliendo...\n");
                break;
            default:
                registrarLog("Opción inválida ingresada en menú principal.");
                printf("\nOpción invalida. Intente de nuevo.\n");
        }
    } while (opcion != 4);

    sqlite3_close(db);
    return 0;
}
