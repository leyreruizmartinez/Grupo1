#include <stdio.h>
#include <time.h>
#include <string.h>

void registrarLog(const char *mensaje) {
    FILE *logFile = fopen("datos/app.log", "a");
    if (logFile == NULL) {
        fprintf(stderr, "Error al abrir el archivo de log.\n");
        return;
    }

    // Obtener la hora actual
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char timeStr[26];
    strftime(timeStr, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    // Escribir la hora y el mensaje en el archivo de log
    fprintf(logFile, "[%s] %s\n", timeStr, mensaje);
    fclose(logFile);
}
