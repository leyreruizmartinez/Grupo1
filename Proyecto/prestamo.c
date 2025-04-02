#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "prestamo.h"
#include "libro.h"
#include "bd.h"

#define MAX_LINEA 256

void obtener_fecha_actual(char* fecha_prestamo, char* fecha_devolucion) {
    // Obtener la fecha actual
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // Formatear la fecha de préstamo (hoy)
    sprintf(fecha_prestamo, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    tm.tm_mon += 1;

    if (tm.tm_mon > 11) {
        tm.tm_mon = 0;
        tm.tm_year += 1;
    }

    sprintf(fecha_devolucion, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

void pedir_libro(int id_usuario, char* isbn) {
    // Contar los préstamos atrasados
    int prestamos_atrasados = contar_prestamos_atrasados(id_usuario);
    if (prestamos_atrasados > 3) {
        printf("Estas sancionado. No puedes pedir mas libros hasta que devuelvas los atrasados.\n");
        return;
    }

    int num_libros;
    Libro* libros = leerFicheroLibros("libros.csv", &num_libros);

    int encontrado = 0;
    for (int i = 0; i < num_libros; i++) {
        if (strcmp(libros[i].isbn, isbn) == 0 && libros[i].copias > 0) {
            printf("Libro encontrado: %s - %s\n", libros[i].isbn, libros[i].titulo);

            libros[i].copias--;

            FILE* archivo_libros = fopen("libros.csv", "w");
            if (archivo_libros == NULL) {
                printf("Error al abrir el archivo de libros\n");
                free(libros);
                return;
            }

            for (int j = 0; j < num_libros; j++) {
                fprintf(archivo_libros, "%s;%s;%s;%d;%d;%d\n",
                        libros[j].isbn, libros[j].titulo, libros[j].autor,
                        libros[j].anyo_publicacion, libros[j].disponible, libros[j].copias);
            }
            fclose(archivo_libros);

            FILE* archivo_historial = fopen("historial.csv", "a");
            if (archivo_historial == NULL) {
                printf("Error al abrir el archivo de historial\n");
                free(libros);
                return;
            }

            char fecha_prestamo[11], fecha_devolucion[11];
            obtener_fecha_actual(fecha_prestamo, fecha_devolucion);

            fprintf(archivo_historial, "%d;%s;%s;%s;%s;%s;0\n", id_usuario, libros[i].isbn, libros[i].titulo, libros[i].autor, fecha_prestamo, fecha_devolucion);
            fclose(archivo_historial);

            FILE* archivo_prestamo = fopen("prestamos.csv", "a");
            if (archivo_prestamo == NULL) {
                printf("Error al abrir el archivo de prestamo\n");
                free(libros);
                return;
            }

            fprintf(archivo_prestamo, "%d;%s\n", id_usuario, libros[i].isbn);
            fclose(archivo_prestamo);

            printf("Libro prestado exitosamente: %s\n", libros[i].titulo);
            encontrado = 1;
            break;
        }
    }

    free(libros);

    if (!encontrado) {
        printf("No se pudo realizar el prestamo, el libro no esta disponible o no existe.\n");
    }
}

int contar_prestamos_atrasados(int id_usuario) {
    FILE* archivo_historial = fopen("historial.csv", "r");
    if (archivo_historial == NULL) {
        printf("Error al abrir el archivo de historial\n");
        return -1;
    }

    char linea[MAX_LINEA];
    int contador = 0;

    while (fgets(linea, sizeof(linea), archivo_historial)) {
        int usuario_id;
        char libro_isbn[20];
        int estado;

        // Leer los datos de la línea
        sscanf(linea, "%d;%[^;];%*[^;];%*[^;];%*[^;];%*[^;];%d", &usuario_id, libro_isbn, &estado);

        if (usuario_id == id_usuario && estado == 2) {
            contador++;
        }
    }

    fclose(archivo_historial);
    return contador;
}

int contar_libros(Libro* libros) {
    int count = 0;
    while (libros[count].isbn != NULL) {
        count++;
    }
    return count;
}

int buscar_libro_por_isbn(Libro* libros, int total_libros, char* isbn) {
    for (int i = 0; i < total_libros; i++) {
        if (strcmp(libros[i].isbn, isbn) == 0) {
            return i;
        }
    }
    return -1;
}

void registrar_prestamo(int id_usuario, char* isbn) {
    printf("Registrando prestamo para el usuario %d con ISBN %s\n", id_usuario, isbn);
}


//funcion para comparar la fecha actual con la de la devolucion, para saber si hace
//falta una penalizacion o no
int calcular_diferencia_dias(char* fecha1, char* fecha2) {
    struct tm tm1 = {0}, tm2 = {0};
    time_t t1, t2;

    sscanf(fecha1, "%4d-%2d-%2d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday);
    tm1.tm_year -= 1900; //ajustar anyo y mes para q tm lo entienda
    tm1.tm_mon -= 1;

    sscanf(fecha2, "%4d-%2d-%2d", &tm2.tm_year, &tm2.tm_mon, &tm2.tm_mday);
    tm2.tm_year -= 1900; //ajustar otra vez
    tm2.tm_mon -= 1;

    t1 = mktime(&tm1);
    t2 = mktime(&tm2);

    return difftime(t2, t1) / (60 * 60 * 24);  //diferencia d dias
}

void devolver_libro(int id_usuario, char* isbn) {
    int num_libros;
    Libro* libros = leerFicheroLibros("libros.csv", &num_libros);

    int encontrado = 0;
    FILE* archivo_historial = fopen("historial.csv", "r");
    if (archivo_historial == NULL) {
        printf("Error al abrir el archivo de historial\n");
        free(libros);
        return;
    }

    FILE* archivo_temp = fopen("historial_temp.csv", "w");
    if (archivo_temp == NULL) {
        printf("Error al abrir el archivo temporal\n");
        fclose(archivo_historial);
        free(libros);
        return;
    }

    char linea[256];
    while (fgets(linea, sizeof(linea), archivo_historial)) {
        char copia[256];
        strcpy(copia, linea);

        char* token = strtok(copia, ";");
        if (token == NULL) continue;
        int usuario_id = atoi(token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        char libro_isbn[20];
        strcpy(libro_isbn, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        char titulo[100];
        strcpy(titulo, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        char autor[100];
        strcpy(autor, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        char fecha_prestamo[11];
        strcpy(fecha_prestamo, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        char fecha_devolucion[11];
        strcpy(fecha_devolucion, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        int estado = atoi(token);

        if (usuario_id == id_usuario && strcmp(libro_isbn, isbn) == 0 && estado == 0) {
            encontrado = 1;

            //para conseguir la fecha actual para compararla con la d la devol.
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            char fecha_actual[11];
            strftime(fecha_actual, 11, "%Y-%m-%d", &tm);

            //calcular la diferencia d dias en si
            int dias_retraso = calcular_diferencia_dias(fecha_devolucion, fecha_actual);

            //ver si esta atrasado o no
            if(dias_retraso > 0){
                printf("el libro esta atrasado por %d dias, se aplicara una penalizacion\n", dias_retraso);
                estado = 2;
            }
            else {
                estado = 1;
            }
            fprintf(archivo_temp, "%d;%s;%s;%s;%s;%s;%i\n", usuario_id, libro_isbn, titulo, autor, fecha_prestamo, fecha_devolucion, estado);
        } else {
            fprintf(archivo_temp, "%s", linea);
        }
    }

    fclose(archivo_historial);
    fclose(archivo_temp);

    if (encontrado) {
        remove("historial.csv");
        rename("historial_temp.csv", "historial.csv");
        printf("Libro devuelto exitosamente.\n");
    } else {
        remove("historial_temp.csv");
        printf("No se encontró el préstamo para este libro.\n");
    }

    free(libros);
}










