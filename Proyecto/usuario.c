#include <stdio.h>
#include <stdlib.h>
#include "usuario.h";


//MEJOR TENER UNA BASE DE DATOS QUITAR SINO
Usuario* leerFicheroLibros(char* nombre_fichero){

    FILE* fichero = fopen(nombre_fichero, "r");
    int max_c = 70;

    if (fichero==NULL){
        printf("Error al abrir el archivo");
    }

    char linea[max_c];
    char c;
    int i=0;

    Usuario* array_usuarios = (Usuario*)malloc(20*sizeof(Usuario));
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
            char* nombre = strdup(campo2); //tenemos que coger en estatico 50 caracteres
            //no es lo mismo copiar que duplicar

            char* campo3 = strtok(NULL, ";");
            char* correo = strdup(campo3);

            char* campo4 = strtok(NULL, ";");
            int telefono;
            sscanf(campo4, "%i", &telefono);

            char* campo5 = strtok(NULL, ";");
            int sancionado;
            sscanf(campo5, "%i", &sancionado);

            Usuario usuario = {id, nombre, correo, telefono, sancionado};
            array_usuarios[index_array]=usuario;
            
            index_array++;
            i = 0;

        }else{
            linea[i] = c;
            i++;
        }
    }

    fclose(fichero);

    return array_usuarios;
}
