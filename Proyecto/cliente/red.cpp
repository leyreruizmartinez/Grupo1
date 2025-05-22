#include "red.h"
#include <iostream>

bool conectarServidor(const char* ip, int puerto, SOCKET &sock) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "Error en WSAStartup\n";
        return false;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "No se pudo crear el socket\n";
        return false;
    }

    sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(puerto);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "No se pudo conectar al servidor\n";
        return false;
    }

    return true;
}

void cerrarConexion(SOCKET sock) {
    closesocket(sock);
    WSACleanup();
}
