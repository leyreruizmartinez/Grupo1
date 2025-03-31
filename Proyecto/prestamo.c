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

    tm.tm_mon += 1;

    if (tm.tm_mon > 11) {
        tm.tm_mon = 0;
        tm.tm_year += 1;
    }

    sprintf(fecha_devolucion, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

void pedir_libro(int id_usuario, char* isbn) {
    int num_libros;
    Libro* libros = leerFicheroLibros("libros.csv", &num_libros);

    int encontrado = 0;
    for (int i = 0; i < num_libros; i++) {
        if (strcmp(libros[i].isbn, isbn) == 0 && libros[i].copias > 0) {
            printf("Libro encontrado: %s - %s\n", libros[i].isbn, libros[i].titulo);

            libros[i].copias--;

            FILE* archivo_libros = fopen("libros.csv", "w");
            if (archivo_libros == NULL) {
                printf("Error al abrir el archivo de libros\n");
                free(libros);
                return;
            }

            for (int j = 0; j < num_libros; j++) {
                fprintf(archivo_libros, "%s;%s;%s;%d;%d;%d\n",
                        libros[j].isbn, libros[j].titulo, libros[j].autor,
                        libros[j].anyo_publicacion, libros[j].disponible, libros[j].copias);
            }
            fclose(archivo_libros);

            FILE* archivo_historial = fopen("historial.csv", "a");
            if (archivo_historial == NULL) {
                printf("Error al abrir el archivo de historial\n");
                free(libros);
                return;
            }

            char fecha_prestamo[11], fecha_devolucion[11];
            obtener_fecha_actual(fecha_prestamo, fecha_devolucion);

            fprintf(archivo_historial, "%d;%s;%s;%s;%s;%s;0\n", id_usuario, libros[i].isbn, libros[i].titulo, libros[i].autor, fecha_prestamo, fecha_devolucion);
            fclose(archivo_historial);

            FILE* archivo_prestamo = fopen("prestamos.csv", "a");
            if (archivo_prestamo == NULL) {
                printf("Error al abrir el archivo de préstamo\n");
                free(libros);
                return;
            }

            fprintf(archivo_prestamo, "%d;%s\n", id_usuario, libros[i].isbn);
            fclose(archivo_prestamo);

            printf("Libro prestado exitosamente: %s\n", libros[i].titulo);
            encontrado = 1;
            break;
        }
    }

    free(libros);

    if (!encontrado) {
        printf("No se pudo realizar el préstamo, el libro no está disponible o no existe.\n");
    }
}

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
    printf("Registrando prestamo para el usuario %d con ISBN %s\n", id_usuario, isbn);
}

void devolver_libro(int id_usuario, char* isbn) {
    int num_libros;
    Libro* libros = leerFicheroLibros("libros.csv", &num_libros);

    int encontrado = 0;
    FILE* archivo_historial = fopen("historial.csv", "r");
    if (archivo_historial == NULL) {
        printf("Error al abrir el archivo de historial\n");
        free(libros);
        return;
    }

    FILE* archivo_temp = fopen("historial_temp.csv", "w");
    if (archivo_temp == NULL) {
        printf("Error al abrir el archivo temporal\n");
        fclose(archivo_historial);
        free(libros);
        return;
    }

    char linea[256];
    while (fgets(linea, sizeof(linea), archivo_historial)) {
        char copia[256];
        strcpy(copia, linea);

        char* token = strtok(copia, ";");
        if (token == NULL) continue;
        int usuario_id = atoi(token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        char libro_isbn[20];
        strcpy(libro_isbn, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        char titulo[100];
        strcpy(titulo, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        char autor[100];
        strcpy(autor, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        char fecha_prestamo[11];
        strcpy(fecha_prestamo, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        char fecha_devolucion[11];
        strcpy(fecha_devolucion, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        int estado = atoi(token);

        if (usuario_id == id_usuario && strcmp(libro_isbn, isbn) == 0 && estado == 0) {
            encontrado = 1;
            fprintf(archivo_temp, "%d;%s;%s;%s;%s;%s;1\n", usuario_id, libro_isbn, titulo, autor, fecha_prestamo, fecha_devolucion);
        } else {
            fprintf(archivo_temp, "%s", linea);
        }
    }

    fclose(archivo_historial);
    fclose(archivo_temp);

    if (encontrado) {
        remove("historial.csv");
        rename("historial_temp.csv", "historial.csv");
        printf("Libro devuelto exitosamente.\n");
    } else {
        remove("historial_temp.csv");
        printf("No se encontró el préstamo para este libro.\n");
    }

    free(libros);
}










