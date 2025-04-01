
#ifndef _REGISTRO_H
#define _REGISTRO_H

// define variables globales ---------------------------------------------
#define MAX_USUARIOS 100
#define MAX_LENGTH 50

// declara la estructura usuario -----------------------------------------
typedef struct {
    int id;
    char nombre[50];
    char apellido[50];
    char correo[100];
    char telefono[20];
    char contrasena[50];
    char sancionado;
} Usuario;


// hace que las variables globales se puedan utilizar en otros módulos ---
extern Usuario usuarios[MAX_USUARIOS];
extern int totalUsuarios;

// cabeceras de las funciones --------------------------------------------
void registrarUsuario();
void mostrarUsuarios();
int validarCorreo(const char *correo);
int validarTelefono(const char *telefono);
void inicializarCSV();
void iniciarSesion();

#endif