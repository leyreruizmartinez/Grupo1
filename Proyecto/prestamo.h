#ifndef PRESTAMO_H
#define PRESTAMO_H

// Definición de la estructura para un préstamo
typedef struct {
    char isbn[14];                 // ISBN del libro (se puede ajustar el tamaño según lo necesario)
    char titulo[100];              // Título del libro
    char fecha_prestamo[11];       // Fecha de préstamo (Formato: YYYY-MM-DD)
    char fecha_devolucion[11];     // Fecha de devolución (Formato: YYYY-MM-DD)
    int estado;                    // Estado del préstamo (0 = activo, 1 = devuelto, 2 = atrasado)
} Prestamo;

// Constantes para los estados del préstamo
#define ESTADO_ACTIVO   0   // El préstamo está activo
#define ESTADO_DEVUELTO 1   // El libro ha sido devuelto
#define ESTADO_ATRASADO 2   // El préstamo está atrasado

// Declaración de funciones
void pedir_libro(int id_usuario);   // Función para pedir un libro en préstamo
void devolver_libro(int id_usuario); // Función para devolver un libro

#endif
