#include <stdio.h>
#include <stdlib.h>
#include <string.h> //para el strtok
#include "libro.h"


void imprimirLibro(Libro libro) {
    printf("[ISBN: %s, Titulo: %s, Autor: %s, Anyo: %d, Disponible: %s] \n", 
           libro.isbn, libro.titulo, libro.autor, libro.anyo_publicacion, 
           libro.disponible ? "Si" : "No"); //en vez de devolver 0 o 1 como está guardado, devolvemos si o no
}

Libro* leerFicheroLibros(char* nombre_fichero) {
    FILE* fichero = fopen(nombre_fichero, "r");
    int max_c = 150;

    if (fichero == NULL) {
        printf("Error: No se pudo abrir el archivo %s\n", nombre_fichero);
        exit(1);
    }

    char linea[max_c];
    char c;
    int i = 0;

    Libro* array_libros = (Libro*)malloc(20 * sizeof(Libro));
    int index = 0;

    while ((c = fgetc(fichero)) != EOF) {
        if (c == '\n') {
            linea[i] = '\0'; //pongo el \0 para indicar el final de la linea
            procesarLinea(linea, &array_libros[index]); //llamo a la funcion de procesar linea con la direccion donde esta el libro del array
            index++;
            i = 0;
        } else {
            linea[i++] = c;
        }
    }

    //si no hacemos esto el ultimo libro del csv no se guarda, entonces tenemos que hacer para cuando no termina con \n
    if (i > 0) {
        linea[i] = '\0';
        procesarLinea(linea, &array_libros[index]);
        index++;
    }

    fclose(fichero);
    return array_libros;
}

// Procesar línea con validaciones de memoria
void procesarLinea(char* linea, Libro* libro) {
    char* campo = strtok(linea, ";");
    libro->isbn = malloc(strlen(campo) + 1);
    if (libro->isbn == NULL) {
        printf("Error al asignar memoria para el ISBN\n");
        exit(1);
    }
    strcpy(libro->isbn, campo);

    char* campo2 = strtok(NULL, ";");
    libro->titulo = malloc(strlen(campo2) + 1);
    if (libro->titulo == NULL) {
        printf("Error al asignar memoria para el título\n");
        exit(1);
    }
    strcpy(libro->titulo, campo2);

    char* campo3 = strtok(NULL, ";");
    libro->autor = malloc(strlen(campo3) + 1);
    if (libro->autor == NULL) {
        printf("Error al asignar memoria para el autor\n");
        exit(1);
    }
    strcpy(libro->autor, campo3);

    char* campo4 = strtok(NULL, ";");
    sscanf(campo4, "%i", &libro->anyo_publicacion);

    char* campo5 = strtok(NULL, ";");
    sscanf(campo5, "%i", &libro->disponible);
}

// Función para liberar la memoria de los libros encontrados
void liberarLibros(Libro* libros, int tamanyo) {
    for (int i = 0; i < tamanyo; i++) {
        free(libros[i].isbn);
        free(libros[i].titulo);
        free(libros[i].autor);
    }
    free(libros);
}

int contarLibrosTitulo(Libro* libros, int tamanyo, char* titulo){
    int contador =0;

    for (int i = 0; i < tamanyo; i++) {
        char* titulo_l = libros[i].titulo;

        if (strstr(titulo_l, titulo) != NULL) { 
            contador++;
        }
    }
    return contador;
}



Libro* buscarLibroTitulo(Libro* libros, int tamanyo, char* titulo){
    int tamanyo_result = contarLibrosTitulo(libros, tamanyo, titulo);
    int len_titulo = strlen(titulo);

    if (tamanyo_result==0){ //significa que no se han encontrado libros con ese titulo
        printf("No se han encontrado resultados de libros para el titulo '%s'\n", titulo);
        return NULL;

    } else{
        Libro* libros_encontrados = (Libro*)malloc(tamanyo_result*sizeof(Libro));

        int indx = 0;
        for (int i=0; i<tamanyo; i++){
            char* titulo_l = libros[i].titulo;

            if (strstr(titulo_l, titulo) != NULL){ //busca una subcadena dentro de otra cadena
                libros_encontrados[indx] = libros[i];
                indx++;
            }
        }

        printf("Se han encontrado los siguientes libros para el titulo '%s':\n", titulo);
        return libros_encontrados;
    }
}



int contarLibrosAutor(Libro* libros, int tamanyo, char* autor){

    int contador =0;
    int len_autor = strlen(autor);

    for (int i=0; i<tamanyo; i++){
        char* autor_l = libros[i].autor;

        if (strstr(autor_l, autor) != NULL){ //si son iguales es 0
            contador++;
        }
    }

    return contador;
}

Libro* buscarLibroAutor(Libro* libros, int tamanyo, char* autor){

    int tamanyo_result = contarLibrosAutor(libros, tamanyo, autor);
    int len_autor = strlen(autor);

    if (tamanyo_result==0){ //significa que no se han encontrado libros con ese autor
        printf("No se han encontrado resultados de libros para el autor '%s'\n", autor);
        return NULL;

    } else{
        Libro* libros_encontrados = (Libro*)malloc(tamanyo_result*sizeof(Libro));
    
        int indx = 0;
        for (int i=0; i<tamanyo; i++){
            char* autor_l = libros[i].autor;

            if (strstr(autor_l, autor) != NULL){ 
                libros_encontrados[indx] = libros[i];
                indx++;
            }
        }
        printf("Se han encontrado los siguientes libros del autor '%s':\n", autor);
        return libros_encontrados;
    }
    
}


int contarLibrosISBN(Libro* libros, int tamanyo, char* isbn){
    int contador =0;

    for (int i=0; i<tamanyo; i++){
        if (strcmp(libros[i].isbn, isbn) == 0){ //si son iguales
            contador++;
        }
    }
    return contador;
}

Libro* buscarLibroISBN(Libro* libros, int tamanyo, char* isbn){

    int tamanyo_result = contarLibrosISBN(libros, tamanyo, isbn);

    if (tamanyo_result==0){ //significa que no se han encontrado libros con ese autor
        printf("No se han encontrado resultados de libros para el ISBN: '%s'\n", isbn);
        return NULL;

    } else{
        Libro* libros_encontrados = (Libro*)malloc(tamanyo_result*sizeof(Libro));
    
        int indx = 0;
        for (int i=0; i<tamanyo; i++){

            if (strcmp(libros[i].isbn, isbn) == 0){ //si son iguales
                libros_encontrados[indx] = libros[i];
                indx++;
            }
        }
        printf("Se ha encontrado lo siguiente para el ISBN: '%s'\n", isbn);
        return libros_encontrados;
    }
    
}