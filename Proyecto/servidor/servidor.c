#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "../bd/sqlite3.h"

#include "servidor.h"
#include "protocolo.h"
#include "log.h"
#include "../src/bd.h"
#include "../src/registro.h"
#include "../src/prestamo.h"

#pragma comment(lib, "ws2_32.lib")

char* recibir_string(SOCKET cliente) {
    int len;
    int total_recibido = 0;
    int ret;

    // Recibir tamaño (int)
    while (total_recibido < sizeof(int)) {
        ret = recv(cliente, ((char*)&len) + total_recibido, sizeof(int) - total_recibido, 0);
        if (ret <= 0) return NULL;
        total_recibido += ret;
    }

    if (len <= 0 || len > 10000) return NULL; // Evitar tamaños locos

    char *buffer = (char*)malloc(len + 1);
    if (!buffer) return NULL;

    total_recibido = 0;
    while (total_recibido < len) {
        ret = recv(cliente, buffer + total_recibido, len - total_recibido, 0);
        if (ret <= 0) {
            free(buffer);
            return NULL;
        }
        total_recibido += ret;
    }

    buffer[len] = '\0';
    return buffer;
}

// Función para limpiar saltos de línea y espacios finales
void limpiar_string(char* str) {
    char* p = str;
    while (*p) {
        if (*p == '\n' || *p == '\r') *p = '\0';
        p++;
    }
}

void procesar_cliente(SOCKET cliente) {
    int comando;

    while (1) {
        int recibido = recv(cliente, (char*)&comando, sizeof(int), 0);
        if (recibido <= 0) {
            printf("DEBUG: Cliente desconectado o error al recibir comando\n");
            break;
        }

        printf("DEBUG: comando recibido = %d\n", comando);

        if (comando == CMD_LOGIN) {
            char *correo = recibir_string(cliente);
            char *clave = recibir_string(cliente);

            if (!correo || !clave) {
                int err = RESP_LOGIN_FAIL;
                send(cliente, (char*)&err, sizeof(int), 0);
                free(correo);
                free(clave);
                continue;
            }

            sqlite3 *db = abrirBD();
            if (!db) {
                free(correo); free(clave);
                continue;
            }

            int id_usuario = login_remoto(db, correo, clave);
            int respuesta = (id_usuario >= 0) ? RESP_LOGIN_OK : RESP_LOGIN_FAIL;
            send(cliente, (char*)&respuesta, sizeof(int), 0);

            if (respuesta == RESP_LOGIN_OK) {
                send(cliente, (char*)&id_usuario, sizeof(int), 0);
            }

            registrar_log((respuesta == RESP_LOGIN_OK) ? "LOGIN OK" : "LOGIN FAIL", correo);

            free(correo); free(clave);
            cerrarBD(db);
        }
        else if (comando == CMD_LISTAR_LIBROS) {
            sqlite3 *db = abrirBD();
            if (!db) {
                int err = RESP_ERROR;
                send(cliente, (char*)&err, sizeof(int), 0);
                continue;
            }

            int count = 0;
            sqlite3_stmt *stmt_count;
            const char *sql = "SELECT titulo, autor FROM LibrosBd;";

            if (sqlite3_prepare_v2(db, sql, -1, &stmt_count, NULL) != SQLITE_OK) {
                int err = RESP_ERROR;
                send(cliente, (char*)&err, sizeof(int), 0);
                cerrarBD(db);
                continue;
            }

            while (sqlite3_step(stmt_count) == SQLITE_ROW) {
                count++;
            }
            sqlite3_finalize(stmt_count);

            send(cliente, (char*)&count, sizeof(int), 0);

            sqlite3_stmt *stmt_data;
            if (sqlite3_prepare_v2(db, sql, -1, &stmt_data, NULL) != SQLITE_OK) {
                int err = RESP_ERROR;
                send(cliente, (char*)&err, sizeof(int), 0);
                cerrarBD(db);
                continue;
            }

            while (sqlite3_step(stmt_data) == SQLITE_ROW) {
                const char *titulo = (const char*)sqlite3_column_text(stmt_data, 0);
                const char *autor = (const char*)sqlite3_column_text(stmt_data, 1);

                char buffer[256];
                snprintf(buffer, sizeof(buffer), "%s - %s", titulo, autor);
                int len = (int)strlen(buffer);
                send(cliente, (char*)&len, sizeof(int), 0);
                send(cliente, buffer, len, 0);
            }

            sqlite3_finalize(stmt_data);
            cerrarBD(db);
        }
        else if (comando == CMD_REGISTRAR) {
            char *nombre = recibir_string(cliente);
            char *apellido = recibir_string(cliente);
            char *correo = recibir_string(cliente);
            char *telefono = recibir_string(cliente);
            char *clave = recibir_string(cliente);

            if (!nombre || !apellido || !correo || !telefono || !clave) {
                int err = RESP_ERROR;
                send(cliente, (char*)&err, sizeof(int), 0);
                free(nombre); free(apellido); free(correo); free(telefono); free(clave);
                continue;
            }

            sqlite3 *db = abrirBD();
            if (!db) {
                int err = RESP_ERROR;
                send(cliente, (char*)&err, sizeof(int), 0);
                free(nombre); free(apellido); free(correo); free(telefono); free(clave);
                continue;
            }

            int exito = registrar_usuario_remoto(db, nombre, apellido, correo, telefono, clave);
            int resp = (exito == 1) ? RESP_OK : RESP_ERROR;
            send(cliente, (char*)&resp, sizeof(int), 0);
            registrar_log((exito == 1) ? "REGISTRO OK" : "REGISTRO FAIL", correo);

            free(nombre); free(apellido); free(correo); free(telefono); free(clave);
            cerrarBD(db);
        }
        else if (comando == CMD_PEDIR_LIBRO) {
            int id_usuario;
            if (recv(cliente, (char*)&id_usuario, sizeof(int), 0) <= 0) continue;

            char *isbn = recibir_string(cliente);
            if (!isbn) continue;

            limpiar_string(isbn);

            printf("Pedido libro: usuario=%d, ISBN='%s'\n", id_usuario, isbn);

            sqlite3 *db = abrirBD();
            if (!db) {
                free(isbn);
                continue;
            }

            int resultado = pedir_libro_remoto(db, id_usuario, isbn);
            printf("Resultado pedir_libro_remoto: %d\n", resultado);

            int resp = (resultado == 1) ? 1 : 0;
            send(cliente, (char*)&resp, sizeof(int), 0);

            free(isbn);
            cerrarBD(db);
        }
        else if (comando == CMD_DEVOLVER_LIBRO) {
            int id_usuario;
            if (recv(cliente, (char*)&id_usuario, sizeof(int), 0) <= 0) continue;

            char *isbn = recibir_string(cliente);
            if (!isbn) continue;

            printf("DEBUG: ISBN recibido: '%s'\n", isbn);
            limpiar_string(isbn);

            sqlite3 *db = abrirBD();
            if (!db) {
                free(isbn);
                continue;
            }

            int resultado = devolver_libro_remoto(db, id_usuario, isbn);
            int resp = (resultado == 1) ? 1 : 0;
            send(cliente, (char*)&resp, sizeof(int), 0);

            free(isbn);
            cerrarBD(db);
        }
        else if (comando == CMD_SALIR) {
            printf("DEBUG: Cliente solicito salir\n");
            break;
        }
        else {
            printf("DEBUG: Comando desconocido: %d\n", comando);
            break;
        }
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
        closesocket(cliente);
    }

    closesocket(servidor);
    WSACleanup();
}

int main() {
    iniciar_servidor();
    return 0;
}
