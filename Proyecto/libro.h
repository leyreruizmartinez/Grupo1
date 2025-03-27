#ifndef LIBRO_H_
#define LIBRO_H_

typedef struct{
    int id;
    char* titulo;
    char* autor;
    int anyo_publicacion;
    int disponible;
}Libro;

void imprimirLibro(Libro libro);
Libro* leerFicheroLibros(char* nombre_fichero);

#endif