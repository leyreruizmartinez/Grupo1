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
        printf("Error: No se pudo abrir el archivo de libros.\n");
        return 0;
    }

    printf("Archivo de libros abierto correctamente.\n");

    int i = 0;
    char linea[256]; // Buffer para leer cada línea

    while (fgets(linea, sizeof(linea), archivo)) {
        printf("Leyendo línea: %s", linea); // Muestra la línea completa

        // Intenta leer los datos usando sscanf
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
            printf("Libro encontrado: %s\n", libros[i].titulo);  // Mensaje de depuración
            return i;
        }
    }
    printf("No se encontró el libro con ISBN: %s\n", isbn);  // Mensaje si no se encuentra
    return -1;
}

int obtener_historial(int id_usuario, Prestamo prestamos[]) {
    // Simulate reading from a file/database for historical data
    // Here we just return a fixed number for illustration
    // Replace this with the actual logic to read from a file or DB

    // Example: Simulate some transactions for user with id `id_usuario`
    for (int i = 0; i < 5; i++) {
        prestamos[i].estado = (i % 2 == 0) ? 1 : 0;  // Simulate alternating between active and returned loans
        snprintf(prestamos[i].titulo, sizeof(prestamos[i].titulo), "Libro %d", i + 1);
        snprintf(prestamos[i].fecha_prestamo, sizeof(prestamos[i].fecha_prestamo), "2025-03-%02d", i + 1);
        snprintf(prestamos[i].fecha_devolucion, sizeof(prestamos[i].fecha_devolucion), "2025-04-%02d", i + 1);
    }
    return 5;  // Assuming 5 transactions for this example
}

void actualizar_disponibilidad(char* isbn, int cambio) {
    FILE *fp = fopen("libros.csv", "r+");
    if (!fp) {
        printf("Error al abrir el archivo de libros.\n");
        return;
    }

    // Crear un archivo temporal para almacenar los cambios
    char linea[100];
    char archivo_temp[1000] = "";
    int encontrado = 0;

    // Leer el archivo línea por línea
    while (fgets(linea, sizeof(linea), fp)) {
        char isbn_file[14], titulo[100], autor[100];
        int disponible, copias, anyo;

        // Leer todos los campos de la línea
        sscanf(linea, "%13s;%99[^;];%99[^;];%d;%d;%d", isbn_file, titulo, autor, &anyo, &disponible, &copias);

        // Verificar si es el libro que queremos modificar
        if (strcmp(isbn, isbn_file) == 0) {
            disponible += cambio;  // Actualizar la disponibilidad
            encontrado = 1;
        }

        // Formatear la línea actualizada
        char nueva_linea[200];
        if (encontrado) {
            // Si encontramos el libro, actualizamos su disponibilidad
            snprintf(nueva_linea, sizeof(nueva_linea), "%s;%s;%s;%d;%d;%d\n", isbn_file, titulo, autor, anyo, disponible, copias);
        } else {
            // Si no es el libro a modificar, dejamos la línea como está
            snprintf(nueva_linea, sizeof(nueva_linea), "%s;%s;%s;%d;%d;%d\n", isbn_file, titulo, autor, anyo, disponible, copias);
        }

        // Concatenar la línea al archivo temporal
        strcat(archivo_temp, nueva_linea);
    }

    // Si no se encontró el libro, no modificamos el archivo
    if (!encontrado) {
        printf("Libro con ISBN %s no encontrado.\n", isbn);
        fclose(fp);
        return;
    }

    // Volver a abrir el archivo para escribir la información modificada
    freopen("libros.csv", "w", fp);
    if (!fp) {
        printf("Error al abrir el archivo de libros para escritura.\n");
        return;
    }

    // Escribir el contenido completo al archivo
    fputs(archivo_temp, fp);

    fclose(fp);
}
