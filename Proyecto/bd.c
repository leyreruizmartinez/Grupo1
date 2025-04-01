#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "prestamo.h"
#include "libro.h"

#define MAX_C 150
#define DB_NAME "libros.db"
#define FILENAME "libros.csv"

// Estructura para almacenar la información de un libro

// Función para procesar una línea del CSV
void procesarLineaBD(char* linea, Libro* libro) {
    sscanf(linea, "%[^,],%[^,],%d,%d", libro->titulo, libro->autor, libro->anyo_publicacion, libro->disponible, libro->copias);
}

// Función para insertar los libros en la base de datos
void insertarEnBD(sqlite3* db, Libro* libros, int num_libros) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO LibrosBd (Titulo, Autor, Anio, Disponible, Copias) VALUES (?, ?, ?, ?);";
    
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    for (int i = 0; i < num_libros; i++) {
        sqlite3_bind_text(stmt, 1, libros[i].titulo, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, libros[i].autor, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, libros[i].anyo_publicacion);
        sqlite3_bind_int(stmt, 4, libros[i].disponible);
        sqlite3_bind_int(stmt, 5, libros[i].copias);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("Error al insertar: %s\n", sqlite3_errmsg(db));
        }
        
        sqlite3_reset(stmt);
    }
    
    sqlite3_finalize(stmt);
}

// Función para leer el fichero CSV y almacenarlo en la base de datos
void leerFicheroYGuardarEnBD(char* nombre_fichero, sqlite3* db) {
    FILE* fichero = fopen(nombre_fichero, "r");
    if (!fichero) {
        perror("Error al abrir el fichero");
        exit(1);
    }

    char linea[MAX_C];
    Libro* array_libros = (Libro*)malloc(20 * sizeof(Libro));
    int index = 0, capacidad = 20;

    while (fgets(linea, MAX_C, fichero)) {
        linea[strcspn(linea, "\n")] = 0;  // Eliminar salto de línea
        procesarLinea(linea, &array_libros[index++]);
        
        if (index >= capacidad) {
            capacidad += 20;
            array_libros = (Libro*)realloc(array_libros, capacidad * sizeof(Libro));
        }
    }
    
    fclose(fichero);
    insertarEnBD(db, array_libros, index);
    free(array_libros);
}

// Función para inicializar la base de datos y crear las tablas necesarias
void inicializarBaseDeDatos() {
    sqlite3 *db;
    char *errMsg = 0;
    int rc = sqlite3_open(DB_NAME, &db);
    if (rc) {
        printf("Error al abrir la base de datos: %s\n", sqlite3_errmsg(db));
        return;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS LibrosBd ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "Titulo TEXT,"
                      "Autor TEXT,"
                      "Anio INTEGER,"
                      "Copias INTEGER,"
                      "Disponible INTEGER,"
                      "CREATE TABLE IF NOT EXISTS Prestamos ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "UsuarioID INTEGER,"
                      "ISBN TEXT,"
                      "FechaPrestamo TEXT,"
                      "FechaDevolucion TEXT,"
                      "Devuelto INTEGER DEFAULT 0);";

    rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        printf("Error al crear las tablas: %s\n", errMsg);
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);
}

// Función para pedir un libro y registrar el préstamo
void pedir_libroBD(int id_usuario, char* isbn) {
    sqlite3 *db;
    sqlite3_open(DB_NAME, &db);
    char sql[512];
    sqlite3_stmt *stmt;
    int copias = 0;

    snprintf(sql, sizeof(sql), "SELECT Copias FROM LibrosBd WHERE ID = '%s';", isbn);
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        copias = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (copias > 0) {
        snprintf(sql, sizeof(sql), "UPDATE LibrosBd SET Copias = Copias - 1 WHERE ID = '%s';", isbn);
        sqlite3_exec(db, sql, 0, 0, 0);

        snprintf(sql, sizeof(sql), "INSERT INTO Prestamos (UsuarioID, ISBN, FechaPrestamo, FechaDevolucion, Devuelto) "
                                   "VALUES (%d, '%s', DATE('now'), DATE('now', '+14 days'), 0);", id_usuario, isbn);
        sqlite3_exec(db, sql, 0, 0, 0);

        printf("Libro prestado con éxito.\n");
    } else {
        printf("El libro no está disponible.\n");
    }

    sqlite3_close(db);
}

// Función para mostrar el historial de préstamos de un usuario
void mostrar_historialBD(int id_usuario) {
    sqlite3 *db;
    sqlite3_open(DB_NAME, &db);
    char sql[256];
    sqlite3_stmt *stmt;

    snprintf(sql, sizeof(sql), "SELECT ISBN, FechaPrestamo, FechaDevolucion, Devuelto FROM Prestamos WHERE UsuarioID = %d;", id_usuario);
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("ISBN: %s, Fecha de préstamo: %s, Fecha de devolución: %s, Estado: %d\n",
               sqlite3_column_text(stmt, 0),
               sqlite3_column_text(stmt, 1),
               sqlite3_column_text(stmt, 2),
               sqlite3_column_int(stmt, 3));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}


// Función para obtener el historial de préstamos de un usuario
int obtener_historialBD(int id_usuario, Prestamo prestamos[]) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char sql[256];
    int contador = 0;

    // Abrir la base de datos
    if (sqlite3_open(DB_NAME, &db)) {
        printf("Error al abrir la base de datos: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // Consulta para obtener el historial de préstamos del usuario
    snprintf(sql, sizeof(sql), "SELECT ISBN, Titulo, Autor, FechaPrestamo, FechaDevolucion, Devuelto FROM Prestamos WHERE UsuarioID = %d;", id_usuario);

    // Preparar la consulta
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar la consulta: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    // Ejecutar la consulta y almacenar los resultados en el arreglo de prestamos
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Asignar los valores recuperados de la base de datos a la estructura Prestamo
        snprintf(prestamos[contador].isbn, sizeof(prestamos[contador].isbn), "%s", sqlite3_column_text(stmt, 0));
        snprintf(prestamos[contador].titulo, sizeof(prestamos[contador].titulo), "%s", sqlite3_column_text(stmt, 1));
        snprintf(prestamos[contador].autor, sizeof(prestamos[contador].autor), "%s", sqlite3_column_text(stmt, 2));
        snprintf(prestamos[contador].fecha_prestamo, sizeof(prestamos[contador].fecha_prestamo), "%s", sqlite3_column_text(stmt, 3));
        snprintf(prestamos[contador].fecha_devolucion, sizeof(prestamos[contador].fecha_devolucion), "%s", sqlite3_column_text(stmt, 4));
        prestamos[contador].estado = sqlite3_column_int(stmt, 5);

        contador++;
    }

    // Finalizar la sentencia y cerrar la base de datos
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    // Devolver el número de préstamos encontrados
    return contador;
}
// Función para cargar los libros desde la base de datos
Libro* cargarLibrosDesdeBD(const char* db_nombre, int* num_libros) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc;

    // Abrir la base de datos
    rc = sqlite3_open(db_nombre, &db);
    if (rc) {
        fprintf(stderr, "No se pudo abrir la base de datos: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    // Consulta SQL para obtener todos los libros
    const char *sql = "SELECT titulo, autor, isbn, disponible FROM libros";
    
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "No se pudo preparar la consulta: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    // Contar cuántos libros hay en la base de datos
    *num_libros = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        (*num_libros)++;
    }

    // Volver al inicio de la consulta
    sqlite3_reset(stmt);

    // Reservar memoria para los libros
    Libro *libros = malloc(*num_libros * sizeof(Libro));
    if (!libros) {
        fprintf(stderr, "Error al reservar memoria para los libros\n");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return NULL;
    }

    // Leer los libros desde la base de datos
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* titulo = (const char*)sqlite3_column_text(stmt, 0);
        const char* autor = (const char*)sqlite3_column_text(stmt, 1);
        const char* isbn = (const char*)sqlite3_column_text(stmt, 2);
        int disponible = sqlite3_column_int(stmt, 3);

        // Copiar los valores a la estructura Libro
        snprintf(libros[i].titulo, sizeof(libros[i].titulo), "%s", titulo);
        snprintf(libros[i].autor, sizeof(libros[i].autor), "%s", autor);
        snprintf(libros[i].isbn, sizeof(libros[i].isbn), "%s", isbn);
        libros[i].disponible = disponible;

        i++;
    }

    // Liberar recursos
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return libros;  // Devolver el arreglo de libros
}