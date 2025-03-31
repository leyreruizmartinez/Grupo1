#ifndef LIBRO_H_
#define LIBRO_H_

typedef struct {
    char* isbn;              // ISBN del libro
    char* titulo;            // Título del libro
    char* autor;             // Autor del libro
    int anyo_publicacion;    // Año de publicación
    int disponible;          // Si está disponible (0 = no, 1 = sí)
    int copias;              // Número de copias disponibles
} Libro;

void imprimirLibro(Libro libro);
void procesarLinea(char* linea, Libro* libro);
Libro* leerFicheroLibros(char* nombre_fichero, int *num_libros);

// Funciones de búsqueda
int contarLibrosTitulo(Libro* libros, int tamanyo, char* titulo);
Libro* buscarLibroTitulo(Libro* libros, int tamanyo, char* titulo);

int contarLibrosAutor(Libro* libros, int tamanyo, char* autor);
Libro* buscarLibroAutor(Libro* libros, int tamanyo, char* autor);

int contarLibrosISBN(Libro* libros, int tamanyo, char* isbn);
Libro* buscarLibroISBN(Libro* libros, int tamanyo, char* isbn);

#endif // LIBRO_H_
