#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bd.h"
#include "libro.h"
#include "prestamo.h"

// Function definitions
int cargar_libros(Libro libros[]) {
    FILE *archivo = fopen("libros.csv", "r");
    if (!archivo) {
        printf("Error al abrir el archivo de libros.\n");
        return 0;
    }

    int i = 0;
    while (fscanf(archivo, "%[^;];%[^;];%[^;];%d;%d;%d\n",
                  libros[i].isbn, libros[i].titulo, libros[i].autor,
                  &libros[i].anyo_publicacion, &libros[i].disponible, &libros[i].copias) == 6) {
        i++;
    }
    
    fclose(archivo);
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

int buscar_libro(Libro libros[], int total_libros, char *isbn) {
    for (int i = 0; i < total_libros; i++) {
        if (strcmp(libros[i].isbn, isbn) == 0) {
            return i;
        }
    }
    return -1;
}

int obtener_historial(int id_usuario, Prestamo prestamos[]) {
    // Implement the logic to fetch historical data from file/database
    return 0;  // Replace with actual return value
}

void actualizar_disponibilidad(char *isbn, int cantidad) {
    // Implement availability update logic
}
