#include <stdio.h>
#include <stdlib.h>
#include "libro.h"



int main(void){

    Libro* libros = leerFicheroLibros("libros.csv");

    for (int i= 0; i<20; i++){
        imprimirLibro(libros[i]);
    }

    return 0;
}