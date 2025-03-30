#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "historial.h"
#include "bd.h"
#include "prestamo.h"

int mostrar_historial(int id_usuario, Prestamo prestamos[]) {
    FILE *archivo = fopen("historial.csv", "r");
    if (!archivo) {
        printf("Error al abrir el archivo de historial.\n");
        return 0;
    }

    int i = 0;
    char linea[256];
    while (fgets(linea, sizeof(linea), archivo)) {
        int id_usuario_leido;
        Prestamo p;
    
        // Debug output to print each line
        printf("%s", linea);
    
        // Leemos los datos de cada línea
        int fieldsRead = sscanf(linea, "%d,%[^,],%[^,],%[^,],%[^,],%d", &id_usuario_leido, p.isbn, p.titulo, p.fecha_prestamo, p.fecha_devolucion, &p.estado);
        if (fieldsRead != 6) {
            printf("Error al leer los datos de la línea. Campos leídos: %d\n", fieldsRead);
            continue;  // Salta esta línea si no se leyeron todos los campos correctamente
        }
    
        // Si el id_usuario coincide, lo agregamos al historial
        if (id_usuario_leido == id_usuario) {
            prestamos[i] = p;
            i++;
        }
    }    
    fclose(archivo);
    return i; // Retornamos el número de préstamos encontrados
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
