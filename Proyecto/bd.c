#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bd.h"
#include "libro.h"
#include "prestamo.h"

int cargar_libros(Libro libros[]) {
    FILE *archivo = fopen("libros.csv", "r");
    if (!archivo) {
        printf("Error: No se pudo abrir el archivo de libros.\n");
        return 0;
    }

    printf("Archivo de libros abierto correctamente.\n");

    int i = 0;
    char linea[256];

    while (fgets(linea, sizeof(linea), archivo)) {
        printf("Leyendo línea: %s", linea);

        int leidos = sscanf(linea, "%[^;];%[^;];%[^;];%d;%d;%d",
                            libros[i].isbn, libros[i].titulo, libros[i].autor,
                            &libros[i].anyo_publicacion, &libros[i].disponible, &libros[i].copias);

        if (leidos == 6) {
            printf("Libro cargado: %s (%d copias)\n", libros[i].titulo, libros[i].copias);
            i++;
        } else {
            printf("Error al leer la línea: %s\n", linea);
        }
    }

    fclose(archivo);
    printf("Total de libros cargados: %d\n", i);
    return i;
}

int actualizar_libros(Libro libros[], int total_libros) {
    FILE *archivo = fopen("libros.csv", "w");
    if (!archivo) {
        printf("Error al abrir el archivo para actualizar.\n");
        return 0;
    }

    for (int i = 0; i < total_libros; i++) {
        fprintf(archivo, "%s;%s;%s;%d;%d;%d\n",
                libros[i].isbn, libros[i].titulo, libros[i].autor,
                libros[i].anyo_publicacion, libros[i].disponible, libros[i].copias);
    }

    fclose(archivo);
    return 1;
}

void listar_libros(Libro libros[], int total_libros) {
    printf("Libros disponibles:\n");
    for (int i = 0; i < total_libros; i++) {
        printf("%s - %s (%d) - Copias: %d\n",
               libros[i].isbn, libros[i].titulo, libros[i].anyo_publicacion, libros[i].copias);
    }
}

int buscar_libro(Libro libros[], int total_libros, char isbn[]) {
    for (int i = 0; i < total_libros; i++) {
        if (strcmp(libros[i].isbn, isbn) == 0) {
            printf("Libro encontrado: %s\n", libros[i].titulo);
            return i;
        }
    }
    printf("No se encontro el libro con ISBN: %s\n", isbn);
    return -1;
}

int obtener_historial(int id_usuario, Prestamo prestamos[]) {

    for (int i = 0; i < 5; i++) {
        prestamos[i].estado = (i % 2 == 0) ? 1 : 0;
        snprintf(prestamos[i].titulo, sizeof(prestamos[i].titulo), "Libro %d", i + 1);
        snprintf(prestamos[i].fecha_prestamo, sizeof(prestamos[i].fecha_prestamo), "2025-03-%02d", i + 1);
        snprintf(prestamos[i].fecha_devolucion, sizeof(prestamos[i].fecha_devolucion), "2025-04-%02d", i + 1);
    }
    return 5;
}
