#ifndef USUARIO_H_
#define USUARIO_H_

typedef struct{
    int id;
    char* nombre;
    char* correo;
    int telefono;
    int sancionado;
}Usuario;

#endif