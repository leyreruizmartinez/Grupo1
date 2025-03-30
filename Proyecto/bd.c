#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bd.h"

// Simulando base de datos con datos estáticos
int obtener_historial(int id_usuario, Prestamo prestamos[]) {
    // Simulación de datos cargados
    if (id_usuario == 1002) {
        strcpy(prestamos[0].titulo, "Cien años de soledad");
        strcpy(prestamos[0].fecha_prestamo, "15/03/2025");
        strcpy(prestamos[0].fecha_devolucion, "30/03/2025");
        prestamos[0].estado = 1; // Activo

        strcpy(prestamos[1].titulo, "El señor de los anillos");
        strcpy(prestamos[1].fecha_prestamo, "05/03/2025");
        strcpy(prestamos[1].fecha_devolucion, "20/03/2025");
        prestamos[1].estado = 2; // Atrasado

        return 2; // Cantidad de préstamos cargados
    }
    return 0; // No hay préstamos
}
