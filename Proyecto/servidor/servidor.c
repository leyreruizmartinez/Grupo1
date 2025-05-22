#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#include "servidor.h"
#include "protocolo.h"
#include "log.h"
#include "../src/bd.h"
#include "../src/registro.h"

#pragma comment(lib, "ws2_32.lib")

char* recibir_string(SOCKET cliente) {
    int len;
    if (recv(cliente, (char*)&len, sizeof(int), 0) <= 0) return NULL;

    char *buffer = (char*)malloc(len + 1);
    if (!buffer) return NULL;

    if (recv(cliente, buffer, len, 0) <= 0) {
        free(buffer);
        return NULL;
    }

    buffer[len] = '\0';
    return buffer;
}

void procesar_cliente(SOCKET cliente) {
    int comando;
    if (recv(cliente, (char*)&comando, sizeof(int), 0) <= 0) return;

    printf("[DEBUG] Comando recibido: %d\n", comando);

    if (comando == CMD_LOGIN) {
        char *correo = recibir_string(cliente);
        char *clave = recibir_string(cliente);

        if (!correo || !clave) {
            printf("[ERROR] Error al recibir datos.\n");
            int err = RESP_LOGIN_FAIL;
            send(cliente, (char*)&err, sizeof(int), 0);
            return;
        }

        printf("[DEBUG] Correo recibido: %s\n", correo);
        printf("[DEBUG] Clave recibida: %s\n", clave);

        sqlite3 *db = abrirBD();
        if (!db) {
            printf("[ERROR] No se pudo abrir la base de datos.\n");
            free(correo); free(clave);
            return;
        }

        int id_usuario = login_remoto(db, correo, clave);
        printf("[DEBUG] ID usuario: %d\n", id_usuario);

        int respuesta = (id_usuario >= 0) ? RESP_LOGIN_OK : RESP_LOGIN_FAIL;
        send(cliente, (char*)&respuesta, sizeof(int), 0);

        registrar_log((id_usuario >= 0) ? "LOGIN OK" : "LOGIN FAIL", correo);

        free(correo);
        free(clave);
        cerrarBD(db);
    }
    else if (comando == CMD_LISTAR_LIBROS) {
        sqlite3 *db = abrirBD();
        if (!db) {
            int err = RESP_ERROR;
            send(cliente, (char*)&err, sizeof(int), 0);
            return;
        }

        sqlite3_stmt *stmt;
        const char *sql = "SELECT titulo, autor FROM LibrosBd;";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
            int err = RESP_ERROR;
            send(cliente, (char*)&err, sizeof(int), 0);
            cerrarBD(db);
            return;
        }

        int count = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            count++;
        }

        // Enviar la cantidad de libros
        send(cliente, (char*)&count, sizeof(int), 0);

        // Reiniciar el statement para enviar los datos
        sqlite3_reset(stmt);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *titulo = (const char*)sqlite3_column_text(stmt, 0);
            const char *autor = (const char*)sqlite3_column_text(stmt, 1);

            char buffer[256];
            snprintf(buffer, sizeof(buffer), "%s - %s", titulo, autor);
            int len = strlen(buffer);
            send(cliente, (char*)&len, sizeof(int), 0);
            send(cliente, buffer, len, 0);
        }

        sqlite3_finalize(stmt);
        cerrarBD(db);
    }
    closesocket(cliente);
}

void iniciar_servidor() {
    WSADATA wsa;
    SOCKET servidor, cliente;
    struct sockaddr_in server_addr, client_addr;
    int c;

    WSAStartup(MAKEWORD(2,2), &wsa);

    servidor = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PUERTO_SERVIDOR);

    bind(servidor, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(servidor, 3);

    printf("Servidor escuchando en puerto %d...\n", PUERTO_SERVIDOR);
    registrar_log("SERVIDOR", "Iniciado");

    c = sizeof(struct sockaddr_in);
    while ((cliente = accept(servidor, (struct sockaddr*)&client_addr, &c)) != INVALID_SOCKET) {
        printf("Cliente conectado\n");
        procesar_cliente(cliente);
    }

    closesocket(servidor);
    WSACleanup();
}

int main() {
    iniciar_servidor();
    return 0;
}
