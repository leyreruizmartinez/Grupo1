#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libro.h"

void imprimirLibro(Libro libro) {
    printf("[ISBN: %s, Titulo: %s, Autor: %s, Anyo: %d, Disponible: %s, Copias: %d] \n", 
           libro.isbn, libro.titulo, libro.autor, libro.anyo_publicacion, 
           libro.disponible ? "Si" : "No", libro.copias);
}

Libro* leerFicheroLibros(char* nombre_fichero, int* num_libros) {
    FILE* fichero = fopen(nombre_fichero, "r");
    int max_c = 150;

    if (fichero == NULL) {
        exit(1);
    }

    char linea[max_c];
    char c;
    int i = 0;

    Libro* array_libros = (Libro*)malloc(20 * sizeof(Libro));
    int index = 0;

    while ((c = fgetc(fichero)) != EOF) {
        if (c == '\n') {
            linea[i] = '\0';
            procesarLinea(linea, &array_libros[index]);
            index++;

            if (index >= 20) {
                array_libros = (Libro*)realloc(array_libros, (index + 20) * sizeof(Libro));
            }

            i = 0;
        } else {
            linea[i++] = c;
        }
    }

    if (i > 0) {
        linea[i] = '\0';
        procesarLinea(linea, &array_libros[index]);
        index++;
    }

    *num_libros = index;
    fclose(fichero);
    return array_libros;
}

void procesarLinea(char* linea, Libro* libro) {
    char* campo = strtok(linea, ";");
    libro->isbn = malloc(strlen(campo) + 1);
    if (libro->isbn == NULL) {
        exit(1);
    }
    strcpy(libro->isbn, campo);

    char* campo2 = strtok(NULL, ";");
    libro->titulo = malloc(strlen(campo2) + 1);
    if (libro->titulo == NULL) {
        exit(1);
    }
    strcpy(libro->titulo, campo2);

    char* campo3 = strtok(NULL, ";");
    libro->autor = malloc(strlen(campo3) + 1);
    if (libro->autor == NULL) {
        exit(1);
    }
    strcpy(libro->autor, campo3);

    char* campo4 = strtok(NULL, ";");
    sscanf(campo4, "%i", &libro->anyo_publicacion);

    char* campo5 = strtok(NULL, ";");
    sscanf(campo5, "%i", &libro->disponible);

    char* campo6 = strtok(NULL, ";");
    sscanf(campo6, "%i", &libro->copias);
}

void liberarLibros(Libro* libros, int tamanyo) {
    for (int i = 0; i < tamanyo; i++) {
        free(libros[i].isbn);
        free(libros[i].titulo);
        free(libros[i].autor);
    }
    free(libros);
}

int contarLibrosTitulo(Libro* libros, int tamanyo, char* titulo) {
    int contador = 0;

    for (int i = 0; i < tamanyo; i++) {
        if (strstr(libros[i].titulo, titulo) != NULL) {
            contador++;
        }
    }
    return contador;
}

Libro* buscarLibroTitulo(Libro* libros, int tamanyo, char* titulo) {
    int tamanyo_result = contarLibrosTitulo(libros, tamanyo, titulo);

    if (tamanyo_result == 0) {
        return NULL;
    } else {
        Libro* libros_encontrados = (Libro*)malloc(tamanyo_result * sizeof(Libro));

        int indx = 0;
        for (int i = 0; i < tamanyo; i++) {
            if (strstr(libros[i].titulo, titulo) != NULL) {
                libros_encontrados[indx] = libros[i];
                indx++;
            }
        }
        return libros_encontrados;
    }
}

int contarLibrosAutor(Libro* libros, int tamanyo, char* autor) {
    int contador = 0;

    for (int i = 0; i < tamanyo; i++) {
        if (strstr(libros[i].autor, autor) != NULL) {
            contador++;
        }
    }
    return contador;
}

Libro* buscarLibroAutor(Libro* libros, int tamanyo, char* autor) {
    int tamanyo_result = contarLibrosAutor(libros, tamanyo, autor);

    if (tamanyo_result == 0) {
        return NULL;
    } else {
        Libro* libros_encontrados = (Libro*)malloc(tamanyo_result * sizeof(Libro));

        int indx = 0;
        for (int i = 0; i < tamanyo; i++) {
            if (strstr(libros[i].autor, autor) != NULL) {
                libros_encontrados[indx] = libros[i];
                indx++;
            }
        }
        return libros_encontrados;
    }
}

int contarLibrosISBN(Libro* libros, int tamanyo, char* isbn) {
    int contador = 0;

    for (int i = 0; i < tamanyo; i++) {
        if (strcmp(libros[i].isbn, isbn) == 0) {
            contador++;
        }
    }
    return contador;
}

Libro* buscarLibroISBN(Libro* libros, int tamanyo, char* isbn) {
    int tamanyo_result = contarLibrosISBN(libros, tamanyo, isbn);

    if (tamanyo_result == 0) {
        return NULL;
    } else {
        Libro* libros_encontrados = (Libro*)malloc(tamanyo_result * sizeof(Libro));

        int indx = 0;
        for (int i = 0; i < tamanyo; i++) {
            if (strcmp(libros[i].isbn, isbn) == 0) {
                libros_encontrados[indx] = libros[i];
                indx++;
            }
        }
        return libros_encontrados;
    }
}

void buscarYImprimirPorAutor(Libro* libros, int tamanyo, char* autor) {
    Libro* librosEncontrados = buscarLibroAutor(libros, tamanyo, autor);
    
    if (librosEncontrados == NULL) {
        printf("No se encontraron libros de autor: %s\n", autor);
    } else {
        printf("Libros encontrados por el autor '%s':\n", autor);
        for (int i = 0; i < contarLibrosAutor(libros, tamanyo, autor); i++) {
            imprimirLibro(librosEncontrados[i]);
        }
        free(librosEncontrados);
    }
}
