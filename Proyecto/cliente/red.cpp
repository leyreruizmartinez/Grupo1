#include "red.h"
#include <iostream>

bool conectarServidor(const char* ip, int puerto, SOCKET &sock) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        return false;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        return false;
    }

    sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(puerto);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        return false;
    }

    return true;
}

void enviar_string(SOCKET sock, const std::string& str) {
    int len = (int)str.size();
    send(sock, (char*)&len, sizeof(int), 0);
    send(sock, str.c_str(), len, 0);
}

std::string recibir_string(SOCKET sock) {
    int len = 0;
    if (recv(sock, (char*)&len, sizeof(int), 0) <= 0)
        return {};

    std::string buffer(len, '\0');
    int recibido = 0;
    while (recibido < len) {
        int r = recv(sock, &buffer[recibido], len - recibido, 0);
        if (r <= 0) return {};
        recibido += r;
    }
    return buffer;
}

void cerrarConexion(SOCKET sock) {
    closesocket(sock);
    WSACleanup();
}
