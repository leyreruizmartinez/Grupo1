#include <iostream>
#include <winsock2.h>
#include "protocolo.h"
#include "red.h"

using namespace std;

int main() {
    SOCKET sock;
    if (!conectarServidor("127.0.0.1", 8080, sock)) {
        return 1;
    }

    cout << "Conectado al servidor." << endl;

    int opcion;
    do {
        cout << "\n1. Iniciar sesion\n2. Salir\n3. Listar libros\nSeleccione: ";
        cin >> opcion;

        if (opcion == 1) {
            string correo, clave;
            cout << "Correo: "; cin >> correo;
            cout << "Clave: "; cin >> clave;

            int cmd = CMD_LOGIN;
            send(sock, (char*)&cmd, sizeof(int), 0);

            // Enviar longitud y contenido del correo
            int lenCorreo = correo.length();
            send(sock, (char*)&lenCorreo, sizeof(int), 0);
            send(sock, correo.c_str(), lenCorreo, 0);

            // Enviar longitud y contenido de la clave
            int lenClave = clave.length();
            send(sock, (char*)&lenClave, sizeof(int), 0);
            send(sock, clave.c_str(), lenClave, 0);

            int respuesta;
            recv(sock, (char*)&respuesta, sizeof(int), 0);

            if (respuesta == RESP_LOGIN_OK) {
                cout << "Inicio de sesion correcto.\n";
            } else {
                cout << "Fallo en el inicio de sesion.\n";
            }

        } else if (opcion == 2) {
            int cmd = CMD_SALIR;
            send(sock, (char*)&cmd, sizeof(int), 0);
            break;
        }
        else if (opcion == 3) {  // Opción para listar libros
            int cmd = CMD_LISTAR_LIBROS;
            send(sock, (char*)&cmd, sizeof(int), 0);

            int cantidad;
            recv(sock, (char*)&cantidad, sizeof(int), 0);
            cout << "Libros disponibles (" << cantidad << "):\n";

            for (int i = 0; i < cantidad; ++i) {
                int len;
                recv(sock, (char*)&len, sizeof(int), 0);

                char* buffer = new char[len + 1];
                recv(sock, buffer, len, 0);
                buffer[len] = '\0';

                cout << i + 1 << ". " << buffer << endl;
                delete[] buffer;
            }
        }
    } while (true);

    cerrarConexion(sock);
    cout << "Conexion cerrada.\n";
    return 0;
}
