#ifndef PRESTAMO_H
#define PRESTAMO_H

typedef struct {
    char titulo[100];
    char fecha_prestamo[11];   // Formato YYYY-MM-DD
    char fecha_devolucion[11]; // Formato YYYY-MM-DD
    int estado; // 0 = activo, 1 = devuelto, 2 = atrasado
} Prestamo;

void pedir_libro(int id_usuario);
void devolver_libro(int id_usuario);

#endif
