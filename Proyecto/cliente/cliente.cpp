#include <iostream>
#include <string>
#include <winsock2.h>
#include "red.h"
#include "../servidor/protocolo.h"

#pragma comment(lib, "ws2_32.lib")

int main() {
    SOCKET s;
    if (!conectarServidor("127.0.0.1", PUERTO_SERVIDOR, s)) {
        std::cerr << "No se pudo conectar al servidor\n";
        return 1;
    }

    std::cout << "Conectado al servidor.\n";

    int opcion = -1;
    while (opcion != 0) {
        std::cout << "\nMenu:\n"
                  << "1 - Login\n"
                  << "2 - Registrar usuario\n"
                  << "3 - Listar libros\n"
                  << "4 - Pedir libro\n"
                  << "5 - Devolver libro\n"
                  << "0 - Salir\n"
                  << "Opcion: ";
        std::cin >> opcion;
        std::cin.ignore();

        if (opcion == 1) {
            int cmd = CMD_LOGIN;
            send(s, (char*)&cmd, sizeof(cmd), 0);

            std::string correo, clave;
            std::cout << "Correo: ";
            std::getline(std::cin, correo);
            std::cout << "Clave: ";
            std::getline(std::cin, clave);

            enviar_string(s, correo);
            enviar_string(s, clave);

            int resp = 0;
            recv(s, (char*)&resp, sizeof(resp), 0);

            if (resp == RESP_LOGIN_OK) {
                int id_usuario = 0;
                recv(s, (char*)&id_usuario, sizeof(id_usuario), 0);
                std::cout << "Login correcto, ID usuario: " << id_usuario << "\n";
            } else {
                std::cout << "Login fallido\n";
            }
        }
        else if (opcion == 2) {
            int cmd = CMD_REGISTRAR;
            send(s, (char*)&cmd, sizeof(cmd), 0);

            std::string nombre, apellido, correo, telefono, clave;
            std::cout << "Nombre: "; std::getline(std::cin, nombre);
            std::cout << "Apellido: "; std::getline(std::cin, apellido);
            std::cout << "Correo: "; std::getline(std::cin, correo);
            std::cout << "Telefono: "; std::getline(std::cin, telefono);
            std::cout << "Clave: "; std::getline(std::cin, clave);

            enviar_string(s, nombre);
            enviar_string(s, apellido);
            enviar_string(s, correo);
            enviar_string(s, telefono);
            enviar_string(s, clave);

            int resp = 0;
            recv(s, (char*)&resp, sizeof(resp), 0);
            if (resp == RESP_OK)
                std::cout << "Registro exitoso\n";
            else
                std::cout << "Registro fallido\n";
        }
        else if (opcion == 3) {
            int cmd = CMD_LISTAR_LIBROS;
            send(s, (char*)&cmd, sizeof(cmd), 0);

            int cantidad = 0;
            recv(s, (char*)&cantidad, sizeof(cantidad), 0);
            std::cout << "Libros disponibles (" << cantidad << "):\n";

            for (int i = 0; i < cantidad; i++) {
                std::string info = recibir_string(s);
                if (!info.empty())
                    std::cout << info << "\n";
            }
        }
        else if (opcion == 4) {
            int cmd = CMD_PEDIR_LIBRO;
            send(s, (char*)&cmd, sizeof(cmd), 0);

            int id_usuario;
            std::cout << "Ingrese su ID de usuario: ";
            std::cin >> id_usuario;
            std::cin.ignore();

            send(s, (char*)&id_usuario, sizeof(id_usuario), 0);

            std::string isbn;
            std::cout << "Ingrese ISBN del libro a pedir: ";
            std::getline(std::cin, isbn);
            enviar_string(s, isbn);

            int resp = 0;
            recv(s, (char*)&resp, sizeof(int), 0);
            if (resp == 1)
                std::cout << "Libro pedido correctamente\n";
            else
                std::cout << "Error al pedir libro\n";
        }
        else if (opcion == 5) {
            int cmd = CMD_DEVOLVER_LIBRO;
            send(s, (char*)&cmd, sizeof(cmd), 0);

            int id_usuario;
            std::cout << "Ingrese su ID de usuario: ";
            std::cin >> id_usuario;
            std::cin.ignore();

            send(s, (char*)&id_usuario, sizeof(id_usuario), 0);

            std::string isbn;
            std::cout << "Ingrese ISBN del libro a devolver: ";
            std::getline(std::cin, isbn);
            enviar_string(s, isbn);

            int resultado = 0;
            recv(s, (char*)&resultado, sizeof(resultado), 0);

            if (resultado == 1)
                std::cout << "Libro devuelto con exito\n";
            else
                std::cout << "Error al devolver libro\n";
        }
    }

    closesocket(s);
    WSACleanup();
    return 0;
}
