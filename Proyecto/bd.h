#ifndef BD_H
#define BD_H

#include "libro.h"
#include "prestamo.h"

#define MAX_LIBROS 100

int cargar_libros(Libro libros[]);
int actualizar_libros(Libro libros[], int total_libros);
void listar_libros(Libro libros[], int total_libros);
int buscar_libro(Libro libros[], int total_libros, char *isbn);
int obtener_historial(int id_usuario, Prestamo prestamos[]);  // Add this declaration

#endif
