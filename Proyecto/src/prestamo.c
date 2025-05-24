#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../bd/sqlite3.h"
#include "prestamo.h"
#include "../servidor/protocolo.h"

// Contar préstamos atrasados de un usuario
int contar_prestamos_atrasados(int id_usuario) {
    FILE *archivo = fopen("datos/prestamos.csv", "r");
    if (!archivo) {
        perror("No se pudo abrir prestamos.csv");
        return -1;
    }

    int contador = 0;
    char linea[256];
    while (fgets(linea, sizeof(linea), archivo)) {
        int usuario_id, estado;
        sscanf(linea, "%d;%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%d", &usuario_id, &estado);

        if (usuario_id == id_usuario && estado == ESTADO_ATRASADO) {
            contador++;
        }
    }

    fclose(archivo);
    return contador;
}

void obtener_fecha_actual(char* buffer, size_t size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d", tm_info);
}

int pedir_libro(int id_usuario, const char* isbn) {
    int atrasados = contar_prestamos_atrasados(id_usuario);
    if (atrasados < 0) return -1;
    if (atrasados >= 3) {
        printf("No puede pedir mas libros, tiene %d prestamos atrasados.\n", atrasados);
        return -1;
    }

    FILE *archivo_libros = fopen("datos/libros.csv", "r");
    FILE *archivo_temp = fopen("datos/temp.csv", "w");
    if (!archivo_libros || !archivo_temp) {
        perror("Error al abrir libros.csv");
        if (archivo_libros) fclose(archivo_libros);
        if (archivo_temp) fclose(archivo_temp);
        return -1;
    }

    char linea[512];
    int encontrado = 0;
    char titulo[100], autor[100];
    int anio, copias, disponible;

    while (fgets(linea, sizeof(linea), archivo_libros)) {
        char curr_isbn[20], curr_titulo[100], curr_autor[100];
        int curr_anio, curr_copias, curr_disponible;

        sscanf(linea, "%[^;];%[^;];%[^;];%d;%d;%d", curr_isbn, curr_titulo, curr_autor, &curr_anio, &curr_disponible, &curr_copias);

        if (strcmp(curr_isbn, isbn) == 0) {
            if (curr_copias <= 0) {
                printf("No hay copias disponibles para el libro %s.\n", isbn);
                fclose(archivo_libros);
                fclose(archivo_temp);
                remove("datos/temp.csv");
                return -1;
            }
            curr_copias--;
            curr_disponible = (curr_copias > 0) ? 1 : 0;
            strcpy(titulo, curr_titulo);
            strcpy(autor, curr_autor);
            anio = curr_anio;
            copias = curr_copias;
            disponible = curr_disponible;
            encontrado = 1;
        }

        fprintf(archivo_temp, "%s;%s;%s;%d;%d;%d\n", curr_isbn, curr_titulo, curr_autor, curr_anio, curr_disponible, curr_copias);
    }

    fclose(archivo_libros);
    fclose(archivo_temp);

    if (!encontrado) {
        printf("Libro con ISBN %s no encontrado.\n", isbn);
        remove("datos/temp.csv");
        return -1;
    }

    if (remove("datos/libros.csv") != 0 || rename("datos/temp.csv", "datos/libros.csv") != 0) {
        perror("Error al actualizar libros.csv");
        return -1;
    }

    char fecha_prestamo[11], fecha_devolucion[11];
    obtener_fecha_actual(fecha_prestamo, sizeof(fecha_prestamo));
    calcular_fecha_devolucion(fecha_prestamo, fecha_devolucion, sizeof(fecha_devolucion));

    FILE *archivo_prestamos = fopen("datos/prestamos.csv", "a");
    if (!archivo_prestamos) {
        perror("Error al abrir prestamos.csv");
        return -1;
    }
    fprintf(archivo_prestamos, "%d;%s\n", id_usuario, isbn);
    fclose(archivo_prestamos);

    FILE *archivo_historial = fopen("datos/historial.csv", "a");
    if (!archivo_historial) {
        perror("Error al abrir historial.csv");
        return -1;
    }
    fprintf(archivo_historial, "%d;%s;%s;%s;%s;%s;%d\n", id_usuario, isbn, titulo, autor, fecha_prestamo, fecha_devolucion, 0);
    fclose(archivo_historial);

    return 0;
}

void calcular_fecha_devolucion(const char *fecha_inicio, char *buffer, size_t size) {
    struct tm tm = {0};
    sscanf(fecha_inicio, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday);
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;

    // Sumar 30 días
    tm.tm_mday += 30;
    mktime(&tm); // Normaliza la fecha

    snprintf(buffer, size, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

int calcular_diferencia_dias(const char* fecha_inicio, const char* fecha_fin) {
    struct tm tm_inicio = {0};
    struct tm tm_fin = {0};

    sscanf(fecha_inicio, "%d-%d-%d", &tm_inicio.tm_year, &tm_inicio.tm_mon, &tm_inicio.tm_mday);
    sscanf(fecha_fin, "%d-%d-%d", &tm_fin.tm_year, &tm_fin.tm_mon, &tm_fin.tm_mday);

    tm_inicio.tm_year -= 1900;
    tm_inicio.tm_mon -= 1;
    tm_fin.tm_year -= 1900;
    tm_fin.tm_mon -= 1;

    time_t t_inicio = mktime(&tm_inicio);
    time_t t_fin = mktime(&tm_fin);

    double diff = difftime(t_fin, t_inicio);
    return (int)(diff / (60 * 60 * 24));
}

int devolver_libro(int id_usuario, const char* isbn) {
    FILE *archivo_libros = fopen("datos/libros.csv", "r");
    FILE *archivo_temp = fopen("datos/temp.csv", "w");
    if (!archivo_libros || !archivo_temp) {
        perror("Error al abrir libros.csv");
        if (archivo_libros) fclose(archivo_libros);
        if (archivo_temp) fclose(archivo_temp);
        return -1;
    }

    char linea[512];
    int encontrado = 0;

    while (fgets(linea, sizeof(linea), archivo_libros)) {
        char curr_isbn[20], titulo[100], autor[100];
        int anio, disponible, copias;

        sscanf(linea, "%[^;];%[^;];%[^;];%d;%d;%d", curr_isbn, titulo, autor, &anio, &disponible, &copias);

        if (strcmp(curr_isbn, isbn) == 0) {
            copias++;
            disponible = (copias > 0) ? 1 : 0;
            encontrado = 1;
        }

        fprintf(archivo_temp, "%s;%s;%s;%d;%d;%d\n", curr_isbn, titulo, autor, anio, disponible, copias);
    }

    fclose(archivo_libros);
    fclose(archivo_temp);

    if (!encontrado) {
        printf("Libro con ISBN %s no encontrado en libros.csv.\n", isbn);
        remove("datos/temp.csv");
        return -1;
    }

    if (remove("datos/libros.csv") != 0 || rename("datos/temp.csv", "datos/libros.csv") != 0) {
        perror("Error al actualizar libros.csv");
        return -1;
    }

    FILE *archivo_historial = fopen("datos/historial.csv", "r");
    FILE *archivo_temp_hist = fopen("datos/temp_hist.csv", "w");
    if (!archivo_historial || !archivo_temp_hist) {
        perror("Error al abrir historial.csv");
        if (archivo_historial) fclose(archivo_historial);
        if (archivo_temp_hist) fclose(archivo_temp_hist);
        return -1;
    }

    char fecha_actual[11];
    obtener_fecha_actual(fecha_actual, sizeof(fecha_actual));

    while (fgets(linea, sizeof(linea), archivo_historial)) {
        int usuario_id, estado;
        char libro_isbn[20], titulo[100], autor[100], fecha_prestamo[11], fecha_devolucion[11];
        int campos = sscanf(linea, "%d;%[^;];%[^;];%[^;];%[^;];%[^;];%d",
                            &usuario_id, libro_isbn, titulo, autor, fecha_prestamo, fecha_devolucion, &estado);

        if (campos == 7 && usuario_id == id_usuario && strcmp(libro_isbn, isbn) == 0 && estado == 0) {
            int dias = calcular_diferencia_dias(fecha_devolucion, fecha_actual);
            int nuevo_estado = (dias > 0) ? 2 : 1;
            fprintf(archivo_temp_hist, "%d;%s;%s;%s;%s;%s;%d\n", usuario_id, libro_isbn, titulo, autor, fecha_prestamo, fecha_devolucion, nuevo_estado);
        } else {
            fputs(linea, archivo_temp_hist);
        }
    }

    fclose(archivo_historial);
    fclose(archivo_temp_hist);

    if (remove("datos/historial.csv") != 0 || rename("datos/temp_hist.csv", "datos/historial.csv") != 0) {
        perror("Error al actualizar historial.csv");
        return -1;
    }

    return 0;
}

int pedir_libro_remoto(sqlite3 *db, int id_usuario, const char* isbn) {
    int rc;
    sqlite3_stmt *stmt;

    // 1. Verificar si el usuario tiene demasiados préstamos atrasados
    int atrasados = contar_prestamos_atrasados(id_usuario);
    if (atrasados < 0) return -1;
    if (atrasados >= 3) {
        printf("No puede pedir mas libros, tiene %d prestamos atrasados.\n", atrasados);
        return -1;
    }

    // 2. Verificar disponibilidad del libro
    const char *sql_check = "SELECT Titulo, Autor, Copias FROM LibrosBd WHERE isbn = ?";
    rc = sqlite3_prepare_v2(db, sql_check, -1, &stmt, NULL);
    if (rc != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, isbn, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        printf("Libro con ISBN %s no encontrado.\n", isbn);
        return -1;
    }

    // Copiar los valores a buffers antes de cerrar stmt
    char titulo[100], autor[100];
    const unsigned char *titulo_raw = sqlite3_column_text(stmt, 0);
    const unsigned char *autor_raw = sqlite3_column_text(stmt, 1);
    int copias = sqlite3_column_int(stmt, 2);

    strncpy(titulo, titulo_raw ? (const char *)titulo_raw : "", sizeof(titulo));
    strncpy(autor, autor_raw ? (const char *)autor_raw : "", sizeof(autor));
    titulo[sizeof(titulo) - 1] = '\0';
    autor[sizeof(autor) - 1] = '\0';

    sqlite3_finalize(stmt);

    // 3. Restar una copia y actualizar disponibilidad
    const char *sql_update = "UPDATE LibrosBd "
                            "SET Copias = Copias - 1, "
                            "Disponible = CASE WHEN Copias - 1 > 0 THEN 1 ELSE 0 END "
                            "WHERE isbn = ?";
    rc = sqlite3_prepare_v2(db, sql_update, -1, &stmt, NULL);
    if (rc != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, isbn, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;

    // 4. Insertar en Prestamos (log)
    const char *sql_insert_prestamo = "INSERT INTO Prestamos (UsuarioID, ISBN) VALUES (?, ?)";
    rc = sqlite3_prepare_v2(db, sql_insert_prestamo, -1, &stmt, NULL);
    if (rc != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, id_usuario);
    sqlite3_bind_text(stmt, 2, isbn, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;

    // 5. Insertar en historial
    char fecha_prestamo[11], fecha_devolucion[11];
    obtener_fecha_actual(fecha_prestamo, sizeof(fecha_prestamo));
    calcular_fecha_devolucion(fecha_prestamo, fecha_devolucion, sizeof(fecha_devolucion));

    const char *sql_insert_hist = "INSERT INTO historial (usuario, isbn, titulo, autor, fecha_prestamo, fecha_devolucion, estado) "
                                  "VALUES (?, ?, ?, ?, ?, ?, 0)";
    rc = sqlite3_prepare_v2(db, sql_insert_hist, -1, &stmt, NULL);
    if (rc != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, id_usuario);
    sqlite3_bind_text(stmt, 2, isbn, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, titulo, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, autor, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, fecha_prestamo, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, fecha_devolucion, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;

    // 6. Actualizar CSV local
    if (pedir_libro(id_usuario, isbn) != 0) {
        printf("Error al actualizar CSV local.\n");
        return -1;
    }

    return 1;
}

int devolver_libro_remoto(sqlite3 *db, int id_usuario, const char* isbn) {
    int rc;
    sqlite3_stmt *stmt;

    // 1. Verificar que el préstamo exista
    const char *sql_check = "SELECT ID FROM Prestamos WHERE UsuarioID = ? AND ISBN = ?";
    rc = sqlite3_prepare_v2(db, sql_check, -1, &stmt, NULL);
    if (rc != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, id_usuario);
    sqlite3_bind_text(stmt, 2, isbn, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        printf("No se encontro el prestamo activo.\n");
        return -1;
    }

    int id_prestamo = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    // 2. Eliminar préstamo (log)
    const char *sql_del = "DELETE FROM Prestamos WHERE ID = ?";
    rc = sqlite3_prepare_v2(db, sql_del, -1, &stmt, NULL);
    if (rc != SQLITE_OK) return -1;

    sqlite3_bind_int(stmt, 1, id_prestamo);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;

    // 3. Sumar una copia y actualizar disponibilidad
    const char *sql_update = "UPDATE LibrosBd "
                            "SET Copias = Copias + 1, "
                            "Disponible = CASE WHEN Copias + 1 > 0 THEN 1 ELSE 0 END "
                            "WHERE isbn = ?";
    rc = sqlite3_prepare_v2(db, sql_update, -1, &stmt, NULL);
    if (rc != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, isbn, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;

    // 4. Actualizar historial
    char fecha_actual[11];
    obtener_fecha_actual(fecha_actual, sizeof(fecha_actual));

    const char *sql_update_hist = "UPDATE historial SET estado = CASE "
                                  "WHEN julianday(?) > julianday(fecha_devolucion) THEN 2 "
                                  "ELSE 1 END "
                                  "WHERE usuario = ? AND isbn = ? AND estado = 0";

    rc = sqlite3_prepare_v2(db, sql_update_hist, -1, &stmt, NULL);
    if (rc != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, fecha_actual, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, id_usuario);
    sqlite3_bind_text(stmt, 3, isbn, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;

    // 5. Actualizar CSV local
    if (devolver_libro(id_usuario, isbn) != 0) {
        printf("Error al actualizar CSV local.\n");
        return -1;
    }

    return 1;
}
