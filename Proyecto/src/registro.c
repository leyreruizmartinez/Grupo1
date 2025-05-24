// includes ------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "registro.h"
#include "menuPrincipal.h"
#include "bd.h"
#include "usuario.h"

#define FILENAME "datos/usuarios.csv" // define el nombre el archivo para que no se pueda modificar

// INICIALIZAR EL ARCHIVO -> CREARLO SI NO EXISTE -----------------------------------------------------------------------------------------------------------------------
void inicializarCSV() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) { // Si el archivo no existe
        file = fopen(FILENAME, "w"); // Crea el archivo vacío
        if (file) {
            fclose(file);
        } else {
            printf("Error al crear el archivo CSV\n");
        }
    } else {
        fclose(file);
    }
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// CREAR IDs UNICOS -----------------------------------------------------------------------------------------------------------------------------------------------------
int generarID() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) return 1;

    char buffer[256];
    int lastID = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        int id;
        if (sscanf(buffer, "%d,", &id) == 1) {
            lastID = id;
        }
    }
    fclose(file);
    return lastID + 1;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// REGISTRAR UN NUEVO USUARIO -> nombre, apellido, correo, telefono, contraseña -----------------------------------------------------------------------------------------
void registrarUsuario() {
    FILE *file = fopen(FILENAME, "a");
    if (!file) {
        printf("Error al abrir el archivo CSV\n");
        return;
    }

    Usuario user;
    user.id = generarID();
    user.sancionado = 0; // 0 significa que no está sancionado

    printf("\t- Ingrese nombre: ");
    fgets(user.nombre, sizeof(user.nombre), stdin);
    user.nombre[strcspn(user.nombre, "\n")] = 0;

    printf("\t- Ingrese apellido: ");
    fgets(user.apellido, sizeof(user.apellido), stdin);
    user.apellido[strcspn(user.apellido, "\n")] = 0;

    printf("\t- Ingrese correo: ");
    fgets(user.correo, sizeof(user.correo), stdin);
    user.correo[strcspn(user.correo, "\n")] = 0;

    printf("\t- Ingrese telefono: ");
    fgets(user.telefono, sizeof(user.telefono), stdin);
    user.telefono[strcspn(user.telefono, "\n")] = 0;

    printf("\t- Ingrese contrasena: ");
    fgets(user.contrasena, sizeof(user.contrasena), stdin);
    user.contrasena[strcspn(user.contrasena, "\n")] = 0;

    fprintf(file, "%d,%s,%s,%s,%s,%s,%d\n", user.id, user.nombre, user.apellido, user.correo, user.telefono, user.contrasena, user.sancionado);
    fclose(file);
    printf("Usuario registrado: %s %s\n", user.nombre, user.apellido);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// MOSTRAR LOS USUARIOS GUARDADOS/ALMACENADOS EN EL CSV
void mostrarUsuarios() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("Error al abrir el archivo CSV.\n");
        return;
    }

    char buffer[256];
    printf("\nLista de Usuarios:\n");

    while (fgets(buffer, sizeof(buffer), file)) {
        Usuario user;
        sscanf(buffer, "%d,%49[^,],%49[^,],%99[^,],%19[^,],%49[^,],%d",
               &user.id, user.nombre, user.apellido, user.correo, user.telefono, user.contrasena, &user.sancionado);

        printf("\tID: %d | Nombre: %s %s | Correo: %s | Telefono: %s | Sancionado: %s\n",
               user.id, user.nombre, user.apellido, user.correo, user.telefono, user.sancionado ? "Sí" : "No");
    }

    fclose(file);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// INICIAR SESIÓN -> comprueba si la contraseña y correos coinciden
void iniciarSesion() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("Error al abrir el archivo CSV.\n");
        return;
    }

    Usuario user;
    char correo[100], contrasena[50], buffer[256];
    int encontrado = 0;

    printf("\t- Ingrese su correo: ");
    fgets(correo, sizeof(correo), stdin);
    correo[strcspn(correo, "\n")] = 0;

    printf("\t- Ingrese su contrasena: ");
    fgets(contrasena, sizeof(contrasena), stdin);
    contrasena[strcspn(contrasena, "\n")] = 0;

    // Leer archivo línea por línea
    int firstLine = 0;  
    while (fgets(buffer, sizeof(buffer), file)) {
        if (firstLine) {  
            firstLine = 0;  
            continue;  // Omitir encabezado si lo tiene
        }

        // Parsear la línea al struct Usuario
        sscanf(buffer, "%d,%49[^,],%49[^,],%99[^,],%19[^,],%49[^,],%s",
               &user.id, user.nombre, user.apellido, user.correo, user.telefono, user.contrasena, &user.sancionado);

        // Comparar credenciales
        if (strcmp(correo, user.correo) == 0 && strcmp(contrasena, user.contrasena) == 0) {
            encontrado = 1;
            printf("\nInicio de sesion exitoso. Bienvenido, %s %s!\n", user.nombre, user.apellido);
            menuPrincipal(user.id);
            if (user.sancionado == '1') {
                printf("Advertencia: Su cuenta esta sancionada.\n");
            }         
            break;
        }
    }

    if (!encontrado) {
        printf("\nCorreo o contrasena incorrectos.\n");
    }

    fclose(file);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

int registrar_usuario_remoto(sqlite3 *db, const char *nombre, const char *apellido, const char *correo, const char *telefono, const char *clave) {
    char sql[512];
    snprintf(sql, sizeof(sql),
             "INSERT INTO usuarios (nombre, apellido, correo, telefono, contrasena, sancionado) VALUES ('%s', '%s', '%s', '%s', '%s', 0);",
             nombre, apellido, correo, telefono, clave);

    char *err = NULL;
    if (sqlite3_exec(db, sql, 0, 0, &err) != SQLITE_OK) {
        fprintf(stderr, "Error al registrar usuario: %s\n", err);
        sqlite3_free(err);
        return 0;
    }

    // Escribir todos los usuarios actuales en el CSV
    FILE *file = fopen("datos/usuarios.csv", "w");
    if (!file) {
        printf("Error al abrir el archivo CSV para escritura\n");
        return 1; // Registrado, pero sin poder escribir CSV
    }

    fprintf(file, "id,nombre,apellido,correo,telefono,contrasena,sancionado\n");

    const char *select_sql = "SELECT id, nombre, apellido, correo, telefono, contrasena, sancionado FROM usuarios;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char *nombre_db = (const char *)sqlite3_column_text(stmt, 1);
            const char *apellido_db = (const char *)sqlite3_column_text(stmt, 2);
            const char *correo_db = (const char *)sqlite3_column_text(stmt, 3);
            const char *telefono_db = (const char *)sqlite3_column_text(stmt, 4);
            const char *clave_db = (const char *)sqlite3_column_text(stmt, 5);
            int sancionado = sqlite3_column_int(stmt, 6);

            fprintf(file, "%d,%s,%s,%s,%s,%s,%d\n", id, nombre_db, apellido_db, correo_db, telefono_db, clave_db, sancionado);
        }
    } else {
        printf("Error al preparar la consulta SELECT para actualizar el CSV\n");
    }

    sqlite3_finalize(stmt);
    fclose(file);

    return 1;
}
