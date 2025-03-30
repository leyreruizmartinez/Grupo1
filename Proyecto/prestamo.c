#include <stdio.h>
#include <string.h>
#include "prestamo.h"
#include "libro.h"
#include "bd.h"

void pedir_libro(int id_usuario) {
    Libro libros[MAX_LIBROS];
    int total_libros = cargar_libros(libros);
    if (total_libros == 0) return;

    char isbn[20];
    printf("Ingrese el ISBN del libro que desea pedir: ");
    scanf("%s", isbn);

    int indice = buscar_libro(libros, total_libros, isbn);
    if (indice == -1 || libros[indice].copias == 0) {
        printf("El libro no está disponible.\n");
        return;
    }

    libros[indice].copias--;
    libros[indice].disponible = 1;
    actualizar_libros(libros, total_libros);

    printf("El usuario %d ha pedido el libro '%s'.\n", id_usuario, libros[indice].titulo);
}

void devolver_libro(int id_usuario) {
    FILE *fp = fopen("prestamos.csv", "r+");
    if (!fp) {
        printf("No se pudo abrir el archivo de préstamos.\n");
        return;
    }

    char linea[100];
    int num_prestamos = 0;
    char isbn_prestamos[10][14];  // Máximo 10 libros por usuario

    printf("\n--- Libros en préstamo ---\n");

    // Leer el archivo y mostrar los libros del usuario
    while (fgets(linea, sizeof(linea), fp)) {
        int usuario_id;
        char isbn[14];

        sscanf(linea, "%d;%13s", &usuario_id, isbn);
        if (usuario_id == id_usuario) {
            printf("%d. ISBN: %s\n", num_prestamos + 1, isbn);
            strcpy(isbn_prestamos[num_prestamos], isbn);
            num_prestamos++;
        }
    }

    if (num_prestamos == 0) {
        printf("No tienes libros en préstamo.\n");
        fclose(fp);
        return;
    }

    // Pedir al usuario que seleccione cuál devolver
    int opcion;
    printf("Seleccione el número del libro que desea devolver: ");
    scanf("%d", &opcion);
    getchar();  // Consumir el salto de línea

    if (opcion < 1 || opcion > num_prestamos) {
        printf("Selección inválida.\n");
        fclose(fp);
        return;
    }

    // Reescribir el archivo de préstamos sin el libro seleccionado
    FILE *temp = fopen("temp.csv", "w");
    while (fgets(linea, sizeof(linea), fp)) {
        int usuario_id;
        char isbn[14];

        sscanf(linea, "%d;%13s", &usuario_id, isbn);
        if (!(usuario_id == id_usuario && strcmp(isbn, isbn_prestamos[opcion - 1]) == 0)) {
            fprintf(temp, "%d;%s\n", usuario_id, isbn);
        }
    }

    fclose(fp);
    fclose(temp);
    remove("prestamos.csv");
    rename("temp.csv", "prestamos.csv");

    // Actualizar la disponibilidad del libro en la base de datos
    actualizar_disponibilidad(isbn_prestamos[opcion - 1], 1);

    printf("Libro devuelto con éxito.\n");
}