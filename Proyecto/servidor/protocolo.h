#ifndef PROTOCOLO_H
#define PROTOCOLO_H

// --- Comandos del cliente al servidor ---
#define CMD_LOGIN           1
#define CMD_SALIR           2
#define CMD_REGISTRAR       3
#define CMD_LISTAR_LIBROS   4
#define CMD_PEDIR_LIBRO     5
#define CMD_DEVOLVER_LIBRO  6

#define PUERTO_SERVIDOR 8080

// --- Respuestas del servidor al cliente ---
#define RESP_OK             100
#define RESP_ERROR          101
#define RESP_LOGIN_OK       102
#define RESP_LOGIN_FAIL     103
#define RESP_NO_DISPONIBLE  104
#define RESP_LIBRO_NO_EXISTE 105
#define RESP_DEVOLUCION_OK  106
#define RESP_DEVOLUCION_FAIL 107

// --- Tamaño máximo para mensajes ---
#define BUFFER_SIZE         1024

// --- Estructura de un libro ---
typedef struct {
    int id;
    char titulo[100];
    char autor[100];
    int disponible;
} LibroProtocolo;

#endif // PROTOCOLO_H
