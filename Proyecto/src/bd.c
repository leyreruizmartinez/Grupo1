#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bd/sqlite3.h"

#define MAX_C 150
#define DB_NAME "../bd/libros.db"
#define FILENAME "../bd/libros.csv"

// Definición de la estructura Libro
typedef struct {
    char isbn[14];      // ISBN de 13 caracteres + '\0'
    char titulo[100];   // Título del libro
    char autor[100];    // Autor del libro
    int anyo_publicacion;
    int copias;
    int disponible;
} Libro;

// Función para procesar una línea del CSV
void procesarLineaBD(char* linea, Libro* libro) {
    int result = sscanf(linea, "%[^;];%[^;];%[^;];%d;%d;%d", libro->isbn, libro->titulo, libro->autor, &libro->anyo_publicacion, &libro->copias, &libro->disponible);
    if (result != 6) {
        fprintf(stderr, "Error al procesar la línea: %s\n", linea);
        return;
    }
}

// Función para insertar los libros en la base de datos
void insertarEnBD(sqlite3* db, Libro* libros, int num_libros) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO LibrosBd (ISBN, Titulo, Autor, Anio, Copias, Disponible) VALUES (?, ?, ?, ?, ?, ?);";
    
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    for (int i = 0; i < num_libros; i++) {
        sqlite3_bind_text(stmt, 1, libros[i].isbn, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, libros[i].titulo, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, libros[i].autor, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, libros[i].anyo_publicacion);
        sqlite3_bind_int(stmt, 5, libros[i].copias);
        sqlite3_bind_int(stmt, 6, libros[i].disponible);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("Error al insertar: %s\n", sqlite3_errmsg(db));
        }
        
        sqlite3_reset(stmt);
    }
    
    sqlite3_finalize(stmt);
}

// Función para leer el fichero CSV y almacenarlo en la base de datos
void leerFicheroYGuardarEnBD(const char* nombre_fichero, sqlite3* db) {
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
        procesarLineaBD(linea, &array_libros[index++]);
        
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
void inicializarBaseDeDatos(sqlite3 **db) {
    char *errMsg = 0;
    int rc = sqlite3_open(DB_NAME, db);
    if (rc) {
        printf("Error al abrir la base de datos: %s\n", sqlite3_errmsg(*db));
        return;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS LibrosBd ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "ISBN TEXT UNIQUE,"
                      "Titulo TEXT,"
                      "Autor TEXT,"
                      "Anio INTEGER,"
                      "Copias INTEGER,"
                      "Disponible INTEGER);"

                      "CREATE TABLE IF NOT EXISTS Prestamos ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "UsuarioID INTEGER,"
                      "ISBN TEXT);"
                      
                      "CREATE TABLE IF NOT EXISTS usuarios ("
                      "id INTEGER PRIMARY KEY,"
                      "nombre TEXT,"
                      "apellido TEXT,"
                      "correo TEXT UNIQUE,"
                      "telefono TEXT,"
                      "contrasena TEXT,"
                      "sancionado TEXT);"
                      
                      "CREATE TABLE IF NOT EXISTS historial ("
                      "usuario INTEGER,"
                      "isbn TEXT,"
                      "titulo TEXT,"
                      "autor TEXT,"
                      "fecha_prestamo TEXT,"
                      "fecha_devolucion TEXT,"
                      "estado INTEGER,"
                      "FOREIGN KEY (usuario) REFERENCES usuarios(id) ON DELETE CASCADE,"
                      "PRIMARY KEY (usuario, isbn, fecha_prestamo));";

    rc = sqlite3_exec(*db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        printf("Error al crear las tablas: %s\n", errMsg);
        sqlite3_free(errMsg);
    }
}

// Función para pedir un libro y registrar el préstamo
void pedir_libroBD(int id_usuario, const char* isbn) {
    sqlite3 *db;
    sqlite3_open(DB_NAME, &db);
    char sql[512];
    sqlite3_stmt *stmt;
    int copias = 0;

    snprintf(sql, sizeof(sql), "SELECT Copias FROM LibrosBd WHERE ISBN = '%s';", isbn);
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        copias = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (copias > 0) {
        snprintf(sql, sizeof(sql), "UPDATE LibrosBd SET Copias = Copias - 1 WHERE ISBN = '%s';", isbn);
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
    const char *sql = "SELECT Titulo, Autor, ISBN, Disponible FROM LibrosBd";
    
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

// Función para procesar una línea del CSV de libros
void procesarLineaLibros(char* linea, Libro* libro) {
    int result = sscanf(linea, "%[^;];%[^;];%[^;];%d;%d;%d", libro->isbn, libro->titulo, libro->autor, &libro->anyo_publicacion, &libro->copias, &libro->disponible);
    if (result != 6) {
        fprintf(stderr, "Error al procesar la línea: %s\n", linea);
        return;
    }
}

// Función para cargar los libros desde el archivo CSV a la base de datos
void cargarLibrosDesdeCSV(sqlite3* db, const char* nombre_fichero) {
    FILE* fichero = fopen(nombre_fichero, "r");
    if (!fichero) {
        perror("Error al abrir el archivo");
        return;
    }

    char linea[MAX_C];
    Libro libro;
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO LibrosBd (ISBN, Titulo, Autor, Anio, Disponible, Copias) VALUES (?, ?, ?, ?, ?, ?);";
    
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    while (fgets(linea, MAX_C, fichero)) {
        linea[strcspn(linea, "\n")] = 0; // Eliminar salto de línea
        procesarLineaLibros(linea, &libro);

        sqlite3_bind_text(stmt, 1, libro.isbn, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, libro.titulo, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, libro.autor, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, libro.anyo_publicacion);
        sqlite3_bind_int(stmt, 5, libro.copias);
        sqlite3_bind_int(stmt, 6, libro.disponible);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("Error al insertar libro: %s\n", sqlite3_errmsg(db));
        }

        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    fclose(fichero);
}

// Estructura para el usuario
typedef struct {
    int id;
    char nombre[100];
    char apellido[100];
    char correo[100];
    char telefono[15];
    char contrasena[100];
    char sancionado[10];
} Usuario;

// Función para procesar una línea del CSV de usuarios
void procesarLineaUsuarios(char* linea, Usuario* usuario) {
    sscanf(linea, "%d,%[^,],%[^,],%[^,],%[^,],%[^,],%s", &usuario->id, usuario->nombre, usuario->apellido, usuario->correo, usuario->telefono, usuario->contrasena, usuario->sancionado);
}

// Función para cargar los usuarios desde el archivo CSV a la base de datos
void cargarUsuariosDesdeCSV(sqlite3* db, const char* nombre_fichero) {
    FILE* fichero = fopen(nombre_fichero, "r");
    if (!fichero) {
        perror("Error al abrir el archivo");
        return;
    }

    char linea[MAX_C];
    Usuario usuario;
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO usuarios (id, nombre, apellido, correo, telefono, contrasena, sancionado) VALUES (?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    while (fgets(linea, MAX_C, fichero)) {
        linea[strcspn(linea, "\n")] = 0; // Eliminar salto de línea
        procesarLineaUsuarios(linea, &usuario);

        sqlite3_bind_int(stmt, 1, usuario.id);
        sqlite3_bind_text(stmt, 2, usuario.nombre, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, usuario.apellido, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, usuario.correo, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, usuario.telefono, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, usuario.contrasena, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 7, usuario.sancionado, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("Error al insertar usuario: %s\n", sqlite3_errmsg(db));
        }

        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    fclose(fichero);
}

// Estructura para el historial de préstamo
typedef struct {
    int usuario;
    char isbn[14];
    char titulo[100];
    char autor[100];
    char fecha_prestamo[11];
    char fecha_devolucion[11];
    int estado;
} Historial;

// Función para validar el formato de la fecha (YYYY-MM-DD)
int validarFecha(const char* fecha) {
    if (strlen(fecha) != 10) return 0; // Debe tener 10 caracteres
    if (fecha[4] != '-' || fecha[7] != '-') return 0; // Debe tener guiones en las posiciones correctas

    // Validar que los componentes de la fecha sean números
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue; // Ignorar guiones
        if (fecha[i] < '0' || fecha[i] > '9') return 0; // Debe ser un dígito
    }

    return 1; // Fecha válida
}

// Función para procesar una línea del CSV de historial
void procesarLineaHistorial(char* linea, Historial* historial) {
    sscanf(linea, "%d;%[^;];%[^;];%[^;];%[^;];%[^;];%d", 
           &historial->usuario, 
           historial->isbn, 
           historial->titulo, 
           historial->autor, 
           historial->fecha_prestamo, 
           historial->fecha_devolucion, 
           &historial->estado);

    // Validar las fechas
    if (!validarFecha(historial->fecha_prestamo)) {
        fprintf(stderr, "Fecha de préstamo inválida: %s\n", historial->fecha_prestamo);
    }
    if (!validarFecha(historial->fecha_devolucion)) {
        fprintf(stderr, "Fecha de devolución inválida: %s\n", historial->fecha_devolucion);
    }
}

// Función para cargar el historial desde el archivo CSV a la base de datos
void cargarHistorialDesdeCSV(sqlite3* db, const char* nombre_fichero) {
    FILE* fichero = fopen(nombre_fichero, "r");
    if (!fichero) {
        perror("Error al abrir el archivo");
        return;
    }

    char linea[MAX_C];
    Historial historial;
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO historial (usuario, isbn, titulo, autor, fecha_prestamo, fecha_devolucion, estado) VALUES (?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    while (fgets(linea, MAX_C, fichero)) {
        linea[strcspn(linea, "\n")] = 0; // Eliminar salto de línea
        procesarLineaHistorial(linea, &historial);

        sqlite3_bind_int(stmt, 1, historial.usuario);
        sqlite3_bind_text(stmt, 2, historial.isbn, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, historial.titulo, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, historial.autor, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, historial.fecha_prestamo, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, historial.fecha_devolucion, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 7, historial.estado);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("Error al insertar historial: %s\n", sqlite3_errmsg(db));
        }

        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    fclose(fichero);
}

// Función para cargar préstamos desde un archivo CSV
void cargarPrestamosDesdeCSV(sqlite3 *db, const char *archivo) {
    FILE *file = fopen(archivo, "r");
    if (file == NULL) {
        printf("No se pudo abrir el archivo CSV: %s\n", archivo);
        return;
    }

    char linea[256];  // Buffer para leer una línea del archivo CSV
    char *usuario_str, *isbn_str;
    int usuario;
    char isbn[14];  // ISBN es de longitud 13, más el '\0'

    // Leer línea por línea del archivo CSV
    while (fgets(linea, sizeof(linea), file)) {
        // Eliminar el salto de línea al final de cada línea
        linea[strcspn(linea, "\n")] = 0;

        // Dividir la línea en dos partes: usuario e isbn
        usuario_str = strtok(linea, ";");
        isbn_str = strtok(NULL, ";");

        if (usuario_str && isbn_str) {
            usuario = atoi(usuario_str);  // Convertir el id de usuario a entero
            strncpy(isbn, isbn_str, sizeof(isbn) - 1);  // Copiar el ISBN
            isbn[sizeof(isbn) - 1] = '\0';  // Asegurarse de que ISBN termine con '\0'

            // Insertar los datos en la base de datos
            char sql[256];
            snprintf(sql, sizeof(sql),
                     "INSERT INTO Prestamos (UsuarioID, ISBN) "
                     "VALUES (%d, '%s');",  // Insertar solo el UsuarioID y el ISBN
                     usuario, isbn);

            char *errMsg = NULL;
            int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
            if (rc != SQLITE_OK) {
                printf("Error al insertar en la base de datos: %s\n", errMsg);
                sqlite3_free(errMsg);
            }
        }
    }

    fclose(file);
}