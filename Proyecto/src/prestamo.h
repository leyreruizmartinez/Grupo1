#ifndef PRESTAMO_H
#define PRESTAMO_H

#include "libro.h"
#include "../bd/sqlite3.h"

// Definición estructura préstamo
typedef struct {
    char isbn[14];
    char titulo[100];
    char autor[100];
    char fecha_prestamo[11];
    char fecha_devolucion[11];
    int estado; // 0 = activo, 1 = devuelto, 2 = atrasado
} Prestamo;

#define ESTADO_ACTIVO   0
#define ESTADO_DEVUELTO 1
#define ESTADO_ATRASADO 2

// Funciones
int pedir_libro(int id_usuario, const char* isbn);
int devolver_libro(int id_usuario, const char* isbn);
int pedir_libro_remoto(sqlite3 *db, int id_usuario, const char* isbn);
int devolver_libro_remoto(sqlite3 *db, int id_usuario, const char* isbn);
void calcular_fecha_devolucion(const char* fecha_prestamo, char* fecha_devolucion, size_t size);
int calcular_diferencia_dias(const char* fecha1, const char* fecha2);
void obtener_fecha_actual(char* buffer, size_t size);

// Funciones auxiliares
int contar_prestamos_atrasados(int id_usuario);

#endif
