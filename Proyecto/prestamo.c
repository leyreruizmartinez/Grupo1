#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "prestamo.h"
#include "libro.h"
#include "bd.h"

#define MAX_LINEA 256

void obtener_fecha_actual(char* fecha_prestamo, char* fecha_devolucion) {
    // Obtener la fecha actual
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // Formatear la fecha de préstamo (hoy)
    sprintf(fecha_prestamo, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    // Agregar un mes a la fecha de préstamo para la devolución
    tm.tm_mon += 1; // Aumentamos un mes

    // Si el mes excede diciembre (12), se ajusta el año
    if (tm.tm_mon > 11) {
        tm.tm_mon = 0; // Enero
        tm.tm_year += 1; // Siguiente año
    }

    // Formatear la fecha de devolución
    sprintf(fecha_devolucion, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

// En prestamo.c
void pedir_libro(int id_usuario, char* isbn) {
    int num_libros;
    Libro* libros = leerFicheroLibros("libros.csv", &num_libros);

    int encontrado = 0;
    for (int i = 0; i < num_libros; i++) {
        if (strcmp(libros[i].isbn, isbn) == 0 && libros[i].copias > 0) {
            printf("Libro encontrado: %s - %s\n", libros[i].isbn, libros[i].titulo);

            // Actualizamos la cantidad de copias disponibles
            libros[i].copias--;

            // Guardamos los cambios de nuevo en el archivo
            FILE* archivo_libros = fopen("libros.csv", "w");
            if (archivo_libros == NULL) {
                printf("Error al abrir el archivo de libros\n");
                free(libros);
                return;
            }

            // Escribimos los libros con la nueva cantidad de copias
            for (int j = 0; j < num_libros; j++) {
                fprintf(archivo_libros, "%s;%s;%s;%d;%d;%d\n",
                        libros[j].isbn, libros[j].titulo, libros[j].autor,
                        libros[j].anyo_publicacion, libros[j].disponible, libros[j].copias);
            }
            fclose(archivo_libros);

            // Registrar el préstamo en el archivo de historial
            FILE* archivo_historial = fopen("historial.csv", "a");
            if (archivo_historial == NULL) {
                printf("Error al abrir el archivo de historial\n");
                free(libros);
                return;
            }

            // Obtener la fecha de préstamo y de devolución
            char fecha_prestamo[11], fecha_devolucion[11];
            obtener_fecha_actual(fecha_prestamo, fecha_devolucion);

            // Registrar el préstamo con las fechas correctas
            fprintf(archivo_historial, "%d;%s;%s;%s;%s;%s;0\n", id_usuario, libros[i].isbn, libros[i].titulo, libros[i].autor, fecha_prestamo, fecha_devolucion);
            fclose(archivo_historial);

            // Registrar el préstamo en el archivo de prestamo.csv
            FILE* archivo_prestamo = fopen("prestamos.csv", "a");
            if (archivo_prestamo == NULL) {
                printf("Error al abrir el archivo de préstamo\n");
                free(libros);
                return;
            }

            // Añadir el registro del préstamo: usuario y ISBN
            fprintf(archivo_prestamo, "%d;%s\n", id_usuario, libros[i].isbn);
            fclose(archivo_prestamo);

            printf("Libro prestado exitosamente: %s\n", libros[i].titulo);
            encontrado = 1;
            break;
        }
    }

    free(libros);  // Liberamos la memoria de los libros

    if (!encontrado) {
        printf("No se pudo realizar el préstamo, el libro no está disponible o no existe.\n");
    }
}

// Definiciones de las funciones
int contar_libros(Libro* libros) {
    int count = 0;
    while (libros[count].isbn != NULL) {
        count++;
    }
    return count;
}

int buscar_libro_por_isbn(Libro* libros, int total_libros, char* isbn) {
    for (int i = 0; i < total_libros; i++) {
        if (strcmp(libros[i].isbn, isbn) == 0) {
            return i;
        }
    }
    return -1;
}

void registrar_prestamo(int id_usuario, char* isbn) {
    // Implementa el registro del préstamo aquí, por ejemplo escribiendo en un archivo.
    printf("Registrando prestamo para el usuario %d con ISBN %s\n", id_usuario, isbn);
}

void devolver_libro(int id_usuario, char* isbn) {
    int num_libros;
    Libro* libros = leerFicheroLibros("libros.csv", &num_libros);

    int encontrado = 0;
    for (int i = 0; i < num_libros; i++) {
        if (strcmp(libros[i].isbn, isbn) == 0) {
            // Verificamos si el libro está prestado
            FILE* archivo_historial = fopen("historial.csv", "r+");
            if (archivo_historial == NULL) {
                printf("Error al abrir el archivo de historial\n");
                free(libros);
                return;
            }

            char linea[256];
            long posicion = -1;

            // Leemos todo el archivo de historial
            while (fgets(linea, sizeof(linea), archivo_historial)) {
                char* token = strtok(linea, ";");
                int usuario_id = atoi(token);  // ID del usuario
                token = strtok(NULL, ";");
                char* libro_isbn = token;  // ISBN del libro
                token = strtok(NULL, ";"); // Ignoramos el título
                token = strtok(NULL, ";"); // Ignoramos el autor
                token = strtok(NULL, ";"); // Ignoramos la fecha de préstamo
                token = strtok(NULL, ";"); // Ignoramos la fecha de devolución
                int estado = atoi(strtok(NULL, ";")); // Estado (0: prestado, 1: devuelto)

                if (usuario_id == id_usuario && strcmp(libro_isbn, isbn) == 0 && estado == 0) {
                    // Si encontramos el préstamo, modificamos solo el estado
                    encontrado = 1;
                    posicion = ftell(archivo_historial) - strlen(linea); // Guardamos la posición de la línea
                    break;
                }
            }

            if (encontrado) {
                // Reposicionamos el archivo al inicio de la línea encontrada
                fseek(archivo_historial, posicion, SEEK_SET);

                // Actualizamos el estado a "devuelto" (1)
                char fecha_prestamo[11], fecha_devolucion[11];
                obtener_fecha_actual(fecha_prestamo, fecha_devolucion);

                // Reescribimos solo el estado en el archivo
                fprintf(archivo_historial, "%d;%s;%s;%s;%s;%s;1\n", id_usuario, isbn, libros[i].titulo, libros[i].autor, fecha_prestamo, fecha_devolucion);
                printf("Libro devuelto exitosamente: %s\n", libros[i].titulo);

                // Actualizamos la cantidad de copias disponibles en libros.csv
                libros[i].copias++;

                // Abrimos libros.csv para escribir la cantidad actualizada de copias
                FILE* archivo_libros = fopen("libros.csv", "w");
                if (archivo_libros == NULL) {
                    printf("Error al abrir el archivo de libros\n");
                    fclose(archivo_historial);
                    free(libros);
                    return;
                }

                for (int j = 0; j < num_libros; j++) {
                    fprintf(archivo_libros, "%s;%s;%s;%d;%d;%d\n", libros[j].isbn, libros[j].titulo, libros[j].autor,
                            libros[j].anyo_publicacion, libros[j].disponible, libros[j].copias);
                }

                fclose(archivo_libros);
                fclose(archivo_historial);
                free(libros);
                return;
            }

            fclose(archivo_historial);
            printf("No se encontró el préstamo para este libro.\n");
            free(libros);
            return;
        }
    }

    printf("No se pudo devolver el libro, no se encontró el libro con ISBN: %s\n", isbn);
    free(libros);
}







