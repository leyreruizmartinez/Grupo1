#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "prestamo.h"
#include "libro.h"
#include "bd.h"

// En prestamo.c
void pedir_libro(int id_usuario, char* isbn) {                    
    Libro* libros = leerFicheroLibros("libros.csv");  // Asegúrate de que esta función esté implementada
    int total_libros = contar_libros(libros);         // Necesitas definir esta función para contar los libros

    if (total_libros == 0) {
        printf("No se pudieron cargar los libros.\n");
        return;
    }

    int indice = buscar_libro_por_isbn(libros, total_libros, isbn);  // Debes implementar esta función
    if (indice == -1 || libros[indice].disponible == 0) {
        printf("El libro no está disponible.\n");
        return;
    }

    // Realiza el préstamo
    printf("Libro encontrado: %s\n", libros[indice].titulo);

    // Registrar el préstamo en el archivo
    registrar_prestamo(id_usuario, isbn);  // Esta es una función que necesitarías implementar

    // Actualizamos la disponibilidad
    actualizar_disponibilidad(isbn, -1);  // Disminuir la disponibilidad

    printf("El usuario %d ha pedido el libro '%s'.\n", id_usuario, libros[indice].titulo);

    // Liberamos memoria
    free(libros);
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
    // Implementación de la función
    printf("Devolviendo el libro con ISBN: %s para el usuario %d\n", isbn, id_usuario);
}