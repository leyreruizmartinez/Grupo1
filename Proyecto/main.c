#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libro.h"



int main(void){

    Libro* libros = leerFicheroLibros("libros.csv");



    //MENU
    char str[10];
    do{
        printf("\n");
        printf("Elige entre 1,... o s (para salir): \n");
        printf("1. Buscar un libro\n"
                "2. Listar  libros disponibles\n"
                ".\n" //seguir poniendo el menu
                ".\n"
                ".\n"
                "Pulsar 's' para salir\n");

        fgets(str, sizeof(str), stdin);
        fflush(stdin);

        //############## BÚSQUEDA DE LIBROS ###############
        if (str[0]=='1'){
            printf("\t1. Buscar por titulo\n"
                "\t2. Buscar por autor\n"
                "\t3. Buscar por ISBN\n");
            
            printf("\t  ");
            char str2[5];
            fgets(str2, sizeof(str2), stdin);
            fflush(stdin);

            //-------------BUSCAR POR TITULO-------------
            if (str2[0]=='1'){
                printf("\tIntroduce el titulo del libro a buscar: ");
                
                char titulo[100];

                fgets(titulo, sizeof(titulo), stdin);
                titulo[strcspn(titulo, "\n")] = '\0';  //elimino el salto de línea
                fflush(stdin); 
                
                int n_libros = contarLibrosTitulo(libros, 20, titulo);
                Libro* array_libros= buscarLibroTitulo(libros, 20,titulo);
                
                if (array_libros != NULL){
                    for (int i=0; i<n_libros; i++){
                        imprimirLibro(array_libros[i]);
                        printf("\n");
                    }
                    free(array_libros);
                    array_libros=NULL;
                }
                
                //-------------BUSCAR POR AUTOR-------------
            } else if(str2[0]=='2'){
                printf("\tIntroduce nombre del autor a buscar: ");
            
                char autor[30];
                fgets(autor, sizeof(autor), stdin);
                autor[strcspn(autor, "\n")] = '\0';  //elimino el salto de línea
                fflush(stdin); 

                int n_libros = contarLibrosAutor(libros, 20, autor );

                Libro* array_libros= buscarLibroAutor(libros, 20,autor);
                if (array_libros != NULL){
                    for (int i=0; i<n_libros; i++){
                        imprimirLibro(array_libros[i]);
                        printf("\n");
                    }
                    free(array_libros);
                    array_libros=NULL;
                }

                //-------------BUSCAR POR ISBN-------------
            } else if(str2[0]=='3'){ 
                printf("\tIntroduce el ISBN completo del libro a buscar: ");

                char isbn[14];
                fgets(isbn, sizeof(isbn), stdin);
                isbn[strcspn(isbn, "\n")] = '\0';  //elimino el salto de línea del isbn
                fflush(stdin); 

                int n_libros = contarLibrosISBN(libros, 20, isbn );
                Libro* array_libros= buscarLibroISBN(libros, 20,isbn);

                if (array_libros != NULL){
                    for (int i=0; i<n_libros; i++){
                        imprimirLibro(array_libros[i]);
                        printf("\n");
                    }
                    free(array_libros);
                    array_libros=NULL;
                }
            } 

            // ################### LISTAR LIBROS DISPONIBLES #####################
        } else if (str[0]=='2') {
            printf("Mostrando libros disponibles...\n");
            for (int i=0; i<20; i++){
                if (libros[i].disponible == 1){
                    imprimirLibro(libros[i]);
                }
            }
            
        }
    } while(str[0]!='s' & str[0]!='a'); //si pulso una 's' es para salir 
    





    //Liberamos memoria
    for (int i = 0; i < 20; i++) {
        free(libros[i].titulo);
        free(libros[i].autor);
    }
    free(libros);
    libros=NULL;

    return 0;
}