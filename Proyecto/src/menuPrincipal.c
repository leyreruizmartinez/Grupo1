#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bd/sqlite3.h"
#include "bd.h"
#include "libro.h"
#include "historial.h"
#include "log.h"

int menuPrincipal(int id_Usuario) {
    sqlite3 *db;
    int opcion;
    int id_usuario = id_Usuario;

    if (sqlite3_open("../datos/libros.db", &db) != SQLITE_OK) {
        printf("No se pudo abrir la base de datos\n");
        registrarLog("Error: No se pudo abrir la base de datos desde menuPrincipal.");
        return 1;
    }
    registrarLog("Base de datos abierta correctamente desde menuPrincipal.");

    Libro* libros = NULL;
    int num_libros = 0;

    libros = cargarLibrosDesdeBD("../datos/libros.db", &num_libros);
    registrarLog("Libros cargados desde la base de datos.");

    char str[10];
    do {
        printf("\n--- MENU PRINCIPAL ---\n");
        printf("1. Buscar un libro\n"
               "2. Listar libros disponibles\n"
               "3. Ver historial de prestamos\n"
               "4. Pedir un libro en prestamo\n"
               "5. Devolver un libro\n"
               "6. Volver atras\n");
        printf("Ingrese una opcion: ");
        fgets(str, sizeof(str), stdin);
        str[strcspn(str, "\n")] = '\0';

        if (str[0] == '1') {
            registrarLog("El usuario accedió a la búsqueda de libros.");
            printf("\t1. Buscar por titulo\n"
                   "\t2. Buscar por autor\n"
                   "\t3. Buscar por ISBN\n");
            printf("\t  ");
            char str2[5];
            fgets(str2, sizeof(str2), stdin);
            str2[strcspn(str2, "\n")] = '\0';

            if (str2[0] == '1') {
                printf("\tIntroduce el titulo del libro a buscar: ");
                char titulo[100];
                fgets(titulo, sizeof(titulo), stdin);
                titulo[strcspn(titulo, "\n")] = '\0';

                char mensaje[200];
                sprintf(mensaje, "Búsqueda por título: %s", titulo);
                registrarLog(mensaje);

                int n_libros = contarLibrosTitulo(libros, num_libros, titulo);
                Libro* array_libros = buscarLibroTitulo(libros, num_libros, titulo);

                if (array_libros != NULL && n_libros > 0) {
                    for (int i = 0; i < n_libros; i++) {
                        imprimirLibro(array_libros[i]);
                        printf("\n");
                    }
                    free(array_libros);
                } else {
                    printf("No se encontraron libros con el título '%s'.\n", titulo);
                }

            } else if (str2[0] == '2') {
                printf("\tIntroduce nombre del autor a buscar: ");
                char autor[30];
                fgets(autor, sizeof(autor), stdin);
                autor[strcspn(autor, "\n")] = '\0';

                char mensaje[200];
                sprintf(mensaje, "Búsqueda por autor: %s", autor);
                registrarLog(mensaje);

                int n_libros = contarLibrosAutor(libros, num_libros, autor);
                Libro* array_libros = buscarLibroAutor(libros, num_libros, autor);

                if (array_libros != NULL && n_libros > 0) {
                    for (int i = 0; i < n_libros; i++) {
                        imprimirLibro(array_libros[i]);
                        printf("\n");
                    }
                    free(array_libros);
                } else {
                    printf("No se encontraron libros de autor '%s'.\n", autor);
                }

            } else if (str2[0] == '3') {
                printf("\tIntroduce el ISBN completo del libro a buscar: ");
                char isbn[14];
                fgets(isbn, sizeof(isbn), stdin);
                isbn[strcspn(isbn, "\n")] = '\0';

                char mensaje[200];
                sprintf(mensaje, "Búsqueda por ISBN: %s", isbn);
                registrarLog(mensaje);

                int n_libros = contarLibrosISBN(libros, num_libros, isbn);
                Libro* array_libros = buscarLibroISBN(libros, num_libros, isbn);

                if (array_libros != NULL && n_libros > 0) {
                    for (int i = 0; i < n_libros; i++) {
                        imprimirLibro(array_libros[i]);
                        printf("\n");
                    }
                    free(array_libros);
                } else {
                    printf("No se encontraron libros con el ISBN '%s'.\n", isbn);
                }
            }

        } else if (str[0] == '2') {
            registrarLog("El usuario solicitó listar libros disponibles.");
            int libros_disponibles = 0;
            for (int i = 0; i < num_libros; i++) {
                if (libros[i].disponible == 1) {
                    imprimirLibro(libros[i]);
                    libros_disponibles++;
                }
            }
            if (libros_disponibles == 0) {
                printf("No hay libros disponibles actualmente.\n");
            }

        } else if (str[0] == '3') {
            registrarLog("El usuario consultó su historial de préstamos.");
            Prestamo prestamos[100];
            mostrar_historial(id_usuario, prestamos);

        } else if (str[0] == '4') {
            Prestamo prestamos[100];
            mostrar_historial(id_usuario, prestamos);
            printf("Ingrese el ISBN del libro que desea pedir: ");
            char isbn[14];
            fgets(isbn, sizeof(isbn), stdin);
            isbn[strcspn(isbn, "\n")] = '\0'; 

            char mensaje[200];
            sprintf(mensaje, "El usuario %d pidió en préstamo el libro con ISBN %s", id_usuario, isbn);
            registrarLog(mensaje);

            pedir_libro(id_usuario, isbn);

        } else if (str[0] == '5') {
            printf("Ingrese el ISBN del libro que desea devolver: ");
            char isbn[14];
            fgets(isbn, sizeof(isbn), stdin);
            isbn[strcspn(isbn, "\n")] = '\0'; 

            char mensaje[200];
            sprintf(mensaje, "El usuario %d devolvió el libro con ISBN %s", id_usuario, isbn);
            registrarLog(mensaje);

            devolver_libro(id_usuario, isbn);

        } else if (str[0] == '6') {
            registrarLog("El usuario salió del menú principal.");
            break;
        } 
    } while (1);

    free(libros);
    sqlite3_close(db);

    printf("\nPresione Enter para salir...\n");
    getchar();

    return 0;
}
