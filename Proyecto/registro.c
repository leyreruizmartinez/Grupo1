// includes ------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "registro.h"
#include "menuPrincipal.h"

#define FILENAME "usuarios.csv" // define el nombre el archivo para que no se pueda modificar

// INICIALIZAR EL ARCHIVO -> CREARLO SI NO EXISTE -----------------------------------------------------------------------------------------------------------------------
void inicializarCSV() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) { // Si el archivo no existe
        file = fopen(FILENAME, "w"); // Crea el archivo vacío
        if (file) {
            fclose(file);
        } else {
            printf("❌ -> Error al crear el archivo CSV\n");
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
        printf("❌ -> Error al abrir el archivo CSV\n");
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

    printf("\t- Ingrese teléfono: ");
    fgets(user.telefono, sizeof(user.telefono), stdin);
    user.telefono[strcspn(user.telefono, "\n")] = 0;

    printf("\t- Ingrese contraseña: ");
    fgets(user.contrasena, sizeof(user.contrasena), stdin);
    user.contrasena[strcspn(user.contrasena, "\n")] = 0;

    fprintf(file, "%d,%s,%s,%s,%s,%s,%d\n", user.id, user.nombre, user.apellido, user.correo, user.telefono, user.contrasena, user.sancionado);
    fclose(file);
    printf("✅ Usuario registrado: %s %s\n", user.nombre, user.apellido);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// MOSTRAR LOS USUARIOS GUARDADOS/ALMACENADOS EN EL CSV
void mostrarUsuarios() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("❌ -> Error al abrir el archivo CSV.\n");
        return;
    }

    char buffer[256];
    printf("\n📋 Lista de Usuarios:\n");

    while (fgets(buffer, sizeof(buffer), file)) {
        Usuario user;
        sscanf(buffer, "%d,%49[^,],%49[^,],%99[^,],%19[^,],%49[^,],%s",
               &user.id, user.nombre, user.apellido, user.correo, user.telefono, user.contrasena, &user.sancionado);

        printf("\tID: %d | Nombre: %s %s | Correo: %s | Teléfono: %s | Sancionado: %s\n",
               user.id, user.nombre, user.apellido, user.correo, user.telefono, user.sancionado ? "No" : "Si");
    }

    fclose(file);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// INICIAR SESIÓN -> comprueba si la contraseña y correos coinciden
void iniciarSesion() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("❌ -> Error al abrir el archivo CSV.\n");
        return;
    }

    Usuario user;
    char correo[100], contrasena[50], buffer[256];
    int encontrado = 0;

    printf("\t- Ingrese su correo: ");
    fgets(correo, sizeof(correo), stdin);
    correo[strcspn(correo, "\n")] = 0;

    printf("\t- Ingrese su contraseña: ");
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
            printf("\n✅ -> Inicio de sesión exitoso. Bienvenido, %s %s!\n", user.nombre, user.apellido);
            menuPrincipal(user.id);
            if (user.sancionado) {
                printf("⚠️ Advertencia: Su cuenta está sancionada.\n");
            }
            break;
        }
    }

    if (!encontrado) {
        printf("\n⛔️ -> Correo o contraseña incorrectos.\n");
    }

    fclose(file);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------
