#ifndef RED_H
#define RED_H

#include <winsock2.h>

bool conectarServidor(const char* ip, int puerto, SOCKET &sock);
void cerrarConexion(SOCKET sock);

#endif
