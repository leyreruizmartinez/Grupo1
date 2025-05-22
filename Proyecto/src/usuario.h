#ifndef USUARIO_H_
#define USUARIO_H_

typedef struct {
    int id;
    char nombre[50];
    char apellido[50];
    char correo[100];
    char telefono[20];
    char contrasena[50];
    char sancionado;
} Usuario;


#endif