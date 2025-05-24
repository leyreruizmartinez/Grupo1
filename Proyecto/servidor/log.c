#include <stdio.h>
#include <time.h>
#include "log.h"

void registrar_log(const char *accion, const char *detalle) {
    FILE *log = fopen("datos/log_servidor.txt", "a");
    if (!log) return;

    time_t ahora = time(NULL);
    struct tm *t = localtime(&ahora);
    fprintf(log, "[%04d-%02d-%02d %02d:%02d:%02d] %s: %s\n",
            t->tm_year+1900, t->tm_mon+1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec,
            accion, detalle);

    fclose(log);
}
