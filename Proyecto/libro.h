#ifndef LIBRO_H_
#define LIBRO_H_

typedef struct {
    char* isbn;              // es un número grande para ser int o long
    char* titulo;
    char* autor;
    int anyo_publicacion; 
    int disponible;  
    int copias;
} Libro;

void imprimirLibro(Libro libro);
void procesarLinea(char* linea, Libro* libro);
Libro* leerFicheroLibros(char* nombre_fichero);

// Funciones de búsqueda
int contarLibrosTitulo(Libro* libros, int tamanyo, char* titulo);
Libro* buscarLibroTitulo(Libro* libros, int tamanyo, char* titulo);

int contarLibrosAutor(Libro* libros, int tamanyo, char* autor);
Libro* buscarLibroAutor(Libro* libros, int tamanyo, char* autor);

int contarLibrosISBN(Libro* libros, int tamanyo, char* isbn);
Libro* buscarLibroISBN(Libro* libros, int tamanyo, char* isbn);

#endif
