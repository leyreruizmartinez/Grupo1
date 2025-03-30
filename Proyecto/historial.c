#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "historial.h"
#include "bd.h" // Para manejar la base de datos

void mostrar_historial(int id_usuario) {
    Prestamo prestamos[50]; // Suponiendo que máximo tiene 50 préstamos
    int total = obtener_historial(id_usuario, prestamos); // Función en bd.c

    if (total == 0) {
        printf("No tiene préstamos registrados.\n");
        return;
    }

    printf("\nHistorial de préstamos del usuario %d:\n", id_usuario);
    for (int i = 0; i < total; i++) {
        printf("- 📖 %s (Prestado el %s, devolución el %s) - Estado: ",
               prestamos[i].titulo, prestamos[i].fecha_prestamo, prestamos[i].fecha_devolucion);
        if (prestamos[i].estado == 0) {
            printf("✅ Devuelto\n");
        } else if (prestamos[i].estado == 1) {
            printf("🟢 Activo\n");
        } else {
            printf("⚠️ Atrasado\n");
        }
    }
}

int tiene_prestamos_atrasados(int id_usuario) {
    Prestamo prestamos[50];
    int total = obtener_historial(id_usuario, prestamos);

    for (int i = 0; i < total; i++) {
        if (prestamos[i].estado == 2) {
            return 1; // Tiene préstamos atrasados
        }
    }
    return 0;
}
