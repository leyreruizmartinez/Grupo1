#ifndef RED_H
#define RED_H

#include <string>
#include <winsock2.h>

bool conectarServidor(const char* ip, int puerto, SOCKET &sock);
void enviar_string(SOCKET sock, const std::string& str);
std::string recibir_string(SOCKET sock);
void cerrarConexion(SOCKET sock);

#endif
