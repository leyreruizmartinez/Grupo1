#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
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

#define DB_NAME "usuarios.db"
#define FILENAME "usuarios.csv"

// Función para inicializar la base de datos y crear la tabla si no existe
void inicializarBaseDeDatos() {
    sqlite3 *db;
    char *errMsg = 0;
    int rc = sqlite3_open(DB_NAME, &db);
    if (rc) {
        printf("Error al abrir la base de datos: %s\n", sqlite3_errmsg(db));
        return;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS usuarios ("
                      "id INTEGER PRIMARY KEY,"
                      "nombre TEXT,"
                      "apellido TEXT,"
                      "correo TEXT UNIQUE,"
                      "telefono TEXT,"
                      "contrasena TEXT,"
                      "sancionado TEXT)";

    rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        printf("Error al crear la tabla: %s\n", errMsg);
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);
}

// Función para importar usuarios desde un archivo CSV a la base de datos
void importarDesdeCSV(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error al abrir el archivo CSV.\n");
        return;
    }

    sqlite3 *db;
    sqlite3_open(DB_NAME, &db);

    char buffer[256];
    int firstLine = 1;
    while (fgets(buffer, sizeof(buffer), file)) {
        if (firstLine) { // Saltar la primera línea (encabezado)
            firstLine = 0;
            continue;
        }
        
        int id;
        char nombre[50], apellido[50], correo[100], telefono[20], contrasena[50], sancionado[10];
        sscanf(buffer, "%d,%49[^,],%49[^,],%99[^,],%19[^,],%49[^,],%9[^\n]",  &id, nombre, apellido, correo, telefono, contrasena, sancionado);
     
        char sql[512];
        snprintf(sql, sizeof(sql), 
                 "INSERT OR IGNORE INTO usuarios (id, nombre, apellido, correo, telefono, contrasena, sancionado) "
                 "VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s');", 
                 id, nombre, apellido, correo, telefono, contrasena, sancionado);
        

        char *errMsg = 0;
        int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            printf("Error al insertar usuario: %s\n", errMsg);
            sqlite3_free(errMsg);
        }
    }
    
    fclose(file);
    sqlite3_close(db);
    printf("\n📝 Datos importados con éxito desde %s\n", filename);
}

