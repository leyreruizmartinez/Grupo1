#include "cliente.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 2048

std::string SERVER_IP = "127.0.0.1";
int SERVER_PORT = 8080;

std::unordered_map<std::string, std::string> cache;
std::ofstream logFile("cliente_log.txt");

void leerConfiguracion(const std::string& archivo) {
    std::ifstream config(archivo);
    std::string linea;
    while (getline(config, linea)) {
        size_t pos = linea.find('=');
        if (pos != std::string::npos) {
            std::string clave = linea.substr(0, pos);
            std::string valor = linea.substr(pos + 1);
            if (clave == "SERVER_IP") SERVER_IP = valor;
            else if (clave == "SERVER_PORT") SERVER_PORT = std::stoi(valor);
        }
    }
}

std::string enviarComando(const std::string& comando) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Error creando socket.\n";
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "No se pudo conectar al servidor.\n";
        close(sock);
        return "ERROR";
    }

    send(sock, comando.c_str(), comando.size(), 0);
    char buffer[BUFFER_SIZE] = {0};
    recv(sock, buffer, BUFFER_SIZE - 1, 0);
    close(sock);

    return std::string(buffer);
}

void menuPrincipal(const std::string& correo) {
    int opcion;
    std::string input;

    do {
        std::cout << "\n---- MENÚ PRINCIPAL ----\n";
        std::cout << "1. Buscar libro por título\n";
        std::cout << "2. Buscar libro por autor\n";
        std::cout << "3. Buscar libro por ISBN\n";
        std::cout << "4. Listar libros disponibles\n";
        std::cout << "5. Ver historial de préstamos\n";
        std::cout << "6. Pedir libro en préstamo\n";
        std::cout << "7. Devolver libro\n";
        std::cout << "8. Ver caché\n";
        std::cout << "9. Salir\n";
        std::cout << "Seleccione una opción: ";
        std::cin >> opcion;
        std::cin.ignore();

        std::string comando, clave;

        switch (opcion) {
            case 1:
                std::cout << "Título: ";
                std::getline(std::cin, input);
                if (cache.count(input)) {
                    std::cout << "[CACHE]\n" << cache[input] << "\n";
                    logFile << "[CACHE HIT] " << input << "\n";
                } else {
                    comando = "BUSCAR_TITULO|" + input;
                    clave = input;
                    std::string resp = enviarComando(comando);
                    cache[clave] = resp;
                    std::cout << resp << "\n";
                    logFile << "[CONSULTA] " << comando << "\n";
                }
                break;
            case 2:
                std::cout << "Autor: ";
                std::getline(std::cin, input);
                comando = "BUSCAR_AUTOR|" + input;
                std::cout << enviarComando(comando) << "\n";
                logFile << "[CONSULTA] " << comando << "\n";
                break;
            case 3:
                std::cout << "ISBN: ";
                std::getline(std::cin, input);
                if (cache.count(input)) {
                    std::cout << "[CACHE]\n" << cache[input] << "\n";
                    logFile << "[CACHE HIT] " << input << "\n";
                } else {
                    comando = "BUSCAR_ISBN|" + input;
                    clave = input;
                    std::string resp = enviarComando(comando);
                    cache[clave] = resp;
                    std::cout << resp << "\n";
                    logFile << "[CONSULTA] " << comando << "\n";
                }
                break;
            case 4:
                std::cout << enviarComando("LISTAR_LIBROS") << "\n";
                logFile << "[CONSULTA] LISTAR_LIBROS\n";
                break;
            case 5:
                comando = "HISTORIAL|" + correo;
                std::cout << enviarComando(comando) << "\n";
                logFile << "[CONSULTA] " << comando << "\n";
                break;
            case 6:
                std::cout << "ISBN del libro a pedir: ";
                std::getline(std::cin, input);
                comando = "PEDIR_PRESTAMO|" + correo + "|" + input;
                std::cout << enviarComando(comando) << "\n";
                logFile << "[SOLICITUD] " << comando << "\n";
                break;
            case 7:
                std::cout << "ISBN del libro a devolver: ";
                std::getline(std::cin, input);
                comando = "DEVOLVER_LIBRO|" + correo + "|" + input;
                std::cout << enviarComando(comando) << "\n";
                logFile << "[DEVOLUCIÓN] " << comando << "\n";
                break;
            case 8:
                std::cout << "=== CACHÉ ===\n";
                for (const auto& [k, v] : cache)
                    std::cout << k << ":\n" << v << "\n";
                break;
            case 9:
                std::cout << "Saliendo...\n";
                logFile << "[SALIDA]\n";
                break;
            default:
                std::cout << "Opción inválida.\n";
        }

    } while (opcion != 9);
}

void menuInicio() {
    int opcion;
    std::string nombre, apellido, correo, telefono, contrasena;

    do {
        std::cout << "\n===== INICIO =====\n";
        std::cout << "1. Registrar usuario\n";
        std::cout << "2. Iniciar sesión\n";
        std::cout << "3. Salir\n";
        std::cout << "Seleccione una opción: ";
        std::cin >> opcion;
        std::cin.ignore();

        if (opcion == 1) {
            std::cout << "Nombre: "; std::getline(std::cin, nombre);
            std::cout << "Apellido: "; std::getline(std::cin, apellido);
            std::cout << "Correo: "; std::getline(std::cin, correo);
            std::cout << "Teléfono: "; std::getline(std::cin, telefono);
            std::cout << "Contraseña: "; std::getline(std::cin, contrasena);

            std::string comando = "REGISTRO|" + nombre + "|" + apellido + "|" + correo + "|" + telefono + "|" + contrasena;
            std::cout << enviarComando(comando) << "\n";
            logFile << "[REGISTRO] " << correo << "\n";

        } else if (opcion == 2) {
            std::cout << "Correo: "; std::getline(std::cin, correo);
            std::cout << "Contraseña: "; std::getline(std::cin, contrasena);

            std::string comando = "LOGIN|" + correo + "|" + contrasena;
            std::string resp = enviarComando(comando);
            std::cout << resp << "\n";

            if (resp.find("éxito") != std::string::npos) {
                logFile << "[LOGIN] " << correo << "\n";
                menuPrincipal(correo);
            }

        } else if (opcion != 3) {
            std::cout << "Opción inválida.\n";
        }

    } while (opcion != 3);
}
