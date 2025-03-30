#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libro.h"
#include "historial.h"  // Nuevo módulo para manejar préstamos
#include "navegacion.h" // Nuevo módulo para opciones de salida

int main(void) {
    Libro* libros = leerFicheroLibros("libros.csv");
    int id_usuario;

    printf("Ingrese su ID de usuario: ");
    scanf("%d", &id_usuario);
    getchar(); // Consumir el salto de línea después del scanf

    // MENÚ PRINCIPAL
    char str[10];
    do {
        printf("\n--- MENÚ PRINCIPAL ---\n");
        printf("1. Buscar un libro\n"
               "2. Listar libros disponibles\n"
               "3. Ver historial de préstamos\n"
               "4. Pedir un libro en préstamo\n"
               "5. Devolver un libro\n"
               "6. Volver atrás\n"
               "7. Volver al menú principal\n"
               "8. Salir\n");
        printf("Ingrese una opción: ");

        fgets(str, sizeof(str), stdin);
        fflush(stdin);

        // ############## BÚSQUEDA DE LIBROS ###############
        if (str[0] == '1') {
            printf("\t1. Buscar por título\n"
                   "\t2. Buscar por autor\n"
                   "\t3. Buscar por ISBN\n");

            printf("\t  ");
            char str2[5];
            fgets(str2, sizeof(str2), stdin);
            fflush(stdin);

            if (str2[0] == '1') {
                printf("\tIntroduce el título del libro a buscar: ");
                char titulo[100];
                fgets(titulo, sizeof(titulo), stdin);
                titulo[strcspn(titulo, "\n")] = '\0'; 
                fflush(stdin);

                int n_libros = contarLibrosTitulo(libros, 20, titulo);
                Libro* array_libros = buscarLibroTitulo(libros, 20, titulo);

                if (array_libros != NULL) {
                    for (int i = 0; i < n_libros; i++) {
                        imprimirLibro(array_libros[i]);
                        printf("\n");
                    }
                    free(array_libros);
                    array_libros = NULL;
                }
            } else if (str2[0] == '2') {
                printf("\tIntroduce nombre del autor a buscar: ");
                char autor[30];
                fgets(autor, sizeof(autor), stdin);
                autor[strcspn(autor, "\n")] = '\0'; 
                fflush(stdin);

                int n_libros = contarLibrosAutor(libros, 20, autor);
                Libro* array_libros = buscarLibroAutor(libros, 20, autor);

                if (array_libros != NULL) {
                    for (int i = 0; i < n_libros; i++) {
                        imprimirLibro(array_libros[i]);
                        printf("\n");
                    }
                    free(array_libros);
                    array_libros = NULL;
                }
            } else if (str2[0] == '3') {
                printf("\tIntroduce el ISBN completo del libro a buscar: ");
                char isbn[14];
                fgets(isbn, sizeof(isbn), stdin);
                isbn[strcspn(isbn, "\n")] = '\0'; 
                fflush(stdin);

                int n_libros = contarLibrosISBN(libros, 20, isbn);
                Libro* array_libros = buscarLibroISBN(libros, 20, isbn);

                if (array_libros != NULL) {
                    for (int i = 0; i < n_libros; i++) {
                        imprimirLibro(array_libros[i]);
                        printf("\n");
                    }
                    free(array_libros);
                    array_libros = NULL;
                }
            }

        // ################### LISTAR LIBROS DISPONIBLES #####################
        } else if (str[0] == '2') {
            printf("Mostrando libros disponibles...\n");
            for (int i = 0; i < 20; i++) {
                if (libros[i].disponible == 1) {
                    imprimirLibro(libros[i]);
                }
            }

        // ################### HISTORIAL DE PRÉSTAMOS #####################
        } else if (str[0] == '3') {
            mostrar_historial(id_usuario);
            if (tiene_prestamos_atrasados(id_usuario)) {
                printf("⚠️ Advertencia: Tiene préstamos vencidos. No puede solicitar más libros.\n");
            }

        // ################### PEDIR UN LIBRO #####################
        } else if (str[0] == '4') {
            if (tiene_prestamos_atrasados(id_usuario)) {
                printf("❌ No puede solicitar más libros hasta devolver los atrasados.\n");
            } else {
                pedir_libro(id_usuario);
            }

        // ################### DEVOLVER UN LIBRO #####################
        } else if (str[0] == '5') {
            devolver_libro(id_usuario);

        // ################### OPCIONES DE NAVEGACIÓN #####################
        } else if (str[0] == '6') {
            volver_atras();
        } else if (str[0] == '7') {
            volver_menu();
        } else if (str[0] == '8') {
            salir_programa();
        }

    } while (str[0] != 's'); // La opción 's' termina el programa.

    // Liberamos memoria
    for (int i = 0; i < 20; i++) {
        free(libros[i].titulo);
        free(libros[i].autor);
    }
    free(libros);
    libros = NULL;

    return 0;
}
