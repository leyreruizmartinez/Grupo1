#ifndef REGISTRO_H_
#define REGISTRO_H_

#include "usuario.h"
#include "../bd/sqlite3.h"

void inicializarCSV();
int generarID();
void registrarUsuario();
void mostrarUsuarios();
void iniciarSesion();

int registrar_usuario_remoto(sqlite3 *db, const char *nombre, const char *apellido, const char *correo, const char *telefono, const char *clave);

#endif
