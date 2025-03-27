#include "libro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //para el strtok


void imprimirLibro(Libro libro) {
    printf("ID: %d, Titulo: %s, Autor: %s, Anyoo: %d, Disponible: %s \n", 
           libro.id, libro.titulo, libro.autor, libro.anyo_publicacion, 
           libro.disponible ? "Si" : "No"); //en vez de devolver 0 o 1 como está guardado, devolvemos si o no
}

Libro* leerFicheroLibros(char* nombre_fichero){

    FILE* fichero = fopen(nombre_fichero, "r");
    int max_c = 70;

    if (fichero==NULL){
        printf("Error al abrir el archivo");
    }

    char linea[max_c];
    char c;
    int i=0;

    Libro* array_libros = (Libro*)malloc(20*sizeof(Libro));
    int index_array = 0;
    
    fgets(linea,max_c, fichero); //descarto la primera fila que es el encabezado

    //Leo linea a linea el archivo csv

    while ((c = fgetc(fichero)) != EOF){
        if (c =='\n'||c == EOF){
            linea[i] = '\0';

            char* campo = strtok(linea, ";");
            int id;
            sscanf(campo, "%i", &id);

            char* campo2 = strtok(NULL, ";");
            char* titulo = strdup(campo2); //tenemos que coger en estatico 50 caracteres
            //no es lo mismo copiar que duplicar

            char* campo3 = strtok(NULL, ";");
            char* autor = strdup(campo3);

            char* campo4 = strtok(NULL, ";");
            int anyo;
            sscanf(campo4, "%i", &anyo);

            char* campo5 = strtok(NULL, ";");
            int disponible;
            sscanf(campo5, "%i", &disponible);

            Libro libro = {id, titulo, autor, anyo, disponible};
            array_libros[index_array]=libro;
            
            index_array++;
            i = 0;

        }else{
            linea[i] = c;
            i++;
        }
    }

    fclose(fichero);

    return array_libros;
}

