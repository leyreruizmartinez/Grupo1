#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "bd.h"
#include "libro.h"
#include "historial.h"

int menuPrincipal(int id_Usuario) {
    sqlite3 *db;
    int opcion;
    int id_usuario = 1;

    // Abrir la base de datos
    if (sqlite3_open("libros.db", &db) != SQLITE_OK) {
        printf("No se pudo abrir la base de datos\n");
        return 1;
    }

    // Cargar los libros desde la base de datos
    Libro* libros = NULL;
    int num_libros = 0;

    // Llamar a la función para cargar los libros desde la base de datos
    libros = cargarLibrosDesdeBD("libros.db", &num_libros);

    // MENU PRINCIPAL
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

        // ############## BUSQUEDA DE LIBROS ###############
        if (str[0] == '1') {
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

        // ################### LISTAR LIBROS DISPONIBLES #####################
        } else if (str[0] == '2') {
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

        // ################### HISTORIAL DE PRESTAMOS #####################
        } else if (str[0] == '3') {
            Prestamo prestamos[100];
            mostrar_historial(id_usuario, prestamos);

        // ################### PEDIR UN LIBRO #####################
        } else if (str[0] == '4') {
            Prestamo prestamos[100];
            mostrar_historial(id_usuario, prestamos);
            printf("Ingrese el ISBN del libro que desea pedir: ");
            char isbn[14];
            fgets(isbn, sizeof(isbn), stdin);
            isbn[strcspn(isbn, "\n")] = '\0'; 
                
            pedir_libro(id_usuario, isbn);
            

        // ################### DEVOLVER UN LIBRO #####################
        } else if (str[0] == '5') {
            printf("Ingrese el ISBN del libro que desea devolver: ");
            char isbn[14];
            fgets(isbn, sizeof(isbn), stdin);
            isbn[strcspn(isbn, "\n")] = '\0'; 

            devolver_libro(id_usuario, isbn);

        // ################### OPCIONES DE NAVEGACION #####################
        } else if (str[0] == '6') {
            break;
        } 
    } while (1);

    // Liberamos memoria
    free(libros);
    libros = NULL;

    // Cerramos la base de datos
    sqlite3_close(db);

    printf("\nPresione Enter para salir...\n");
    getchar();

    return 0;
}
