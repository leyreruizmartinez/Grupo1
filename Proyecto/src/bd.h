#ifndef BD_H
#define BD_H

#include "libro.h"
#include "prestamo.h"
#include "../bd/sqlite3.h"


#define MAX_LIBROS 100

// Funciones para libros
int cargar_libros(Libro libros[]);
int actualizar_libros(Libro libros[], int total_libros);
int listar_libros(Libro** libros_disponibles, int* cantidad);
int buscar_libro(Libro libros[], int total_libros, char *isbn);

// Funciones para historial de préstamos
int obtener_historialBD(int id_usuario, Prestamo prestamos[]);

// Funciones para la base de datos
void inicializarBaseDeDatos(sqlite3 *db);
void importarDesdeCSV(const char *filename);

// Función para registrar un préstamo
void pedir_libroBD(int id_usuario, char* isbn);

// Función para mostrar el historial de préstamos de un usuario
int mostrar_historialBD(int id_usuario, Prestamo prestamos[]);

Libro* leerFicheroLibros(const char *nombre_fichero, int *num_libros);
Libro* cargarLibrosDesdeBD(const char* db_nombre, int* num_libros);
void cargarHistorialDesdeCSV(sqlite3* db, const char* nombre_fichero);
void cargarUsuariosDesdeCSV(sqlite3* db, const char* nombre_fichero);
void cargarLibrosDesdeCSV(sqlite3* db, const char* nombre_fichero);
void cargarPrestamosDesdeCSV(sqlite3 *db, const char *archivo);

sqlite3* abrirBD();
void cerrarBD(sqlite3 *db);
int login_remoto(sqlite3 *db, const char *correo, const char *clave);

void insertar_prestamo_sqlite(sqlite3 *db, int id_usuario, const char* isbn);

int insertar_historial_sqlite(sqlite3 *db, int id_usuario, const char *isbn, const char *titulo, const char *autor, const char *fecha_prestamo, const char *fecha_devolucion, int estado);
int actualizar_estado_historial_sqlite(sqlite3 *db, int id_usuario, const char *isbn, int nuevo_estado);

int obtener_libro_por_isbn(sqlite3 *db, const char *isbn, Libro *libro);
int obtener_estado_devolucion(sqlite3 *db, int id_usuario, const char *isbn, const char *fecha_actual);

int calcular_dias_entre_fechas(const char *fecha1, const char *fecha2);

void actualizar_copias_libro(sqlite3 *db, const char *isbn, int cambio);

#endif
