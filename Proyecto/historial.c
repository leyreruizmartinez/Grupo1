#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "historial.h"
#include "bd.h"
#include "prestamo.h"

#define MAX_LINEA 1024

int mostrar_historial(int id_usuario, Prestamo prestamos[]) {
    FILE *archivo = fopen("historial.csv", "r");
    if (archivo == NULL) {
        printf("Error al abrir el archivo\n");
        return -1;
    }

    char linea[MAX_LINEA];
    int contador = 0;
    
    // Leer las líneas del archivo
    while (fgets(linea, sizeof(linea), archivo)) {
        Prestamo p;
        int id;
        
        int result = sscanf(linea, "%d;%14[^;];%100[^;];%100[^;];%11[^;];%11[^;];%d",
            &id, p.isbn, p.titulo, p.autor, p.fecha_prestamo, p.fecha_devolucion, &p.estado);

        if (result == 7) {
            if (id == id_usuario) {
                printf("ISBN: %s\n", p.isbn);
                printf("Titulo: %s\n", p.titulo);
                printf("Autor: %s\n", p.autor);
                printf("Fecha de prestamo: %s\n", p.fecha_prestamo);
                printf("Fecha de devolucion: %s\n", p.fecha_devolucion);
                printf("Estado: %s\n", p.estado == 0 ? "En prestamo" : p.estado == 1 ? "Devuelto" : "Atrasado");
                printf("-----------------------------\n");
                contador++;
            }
        } else {
            printf("Error al leer la linea: %s (resultados leidos: %d)\n", linea, result);
        }
    }

    fclose(archivo);
    if (contador == 0) {
        printf("No se encontraron prestamos para el usuario %d\n", id_usuario);
    }

    return 0;
}

int tiene_prestamos_atrasados(int id_usuario) {
    Prestamo prestamos[50];
    int total = mostrar_historial(id_usuario, prestamos);

    for (int i = 0; i < total; i++) {
        if (prestamos[i].estado == 2) {
            return 1;
        }
    }
    return 0;
}
