#ifndef HISTORIAL_H
#define HISTORIAL_H

#include "prestamo.h"

// Funciones
int mostrar_historial(int id_usuario, Prestamo prestamos[]);
int tiene_prestamos_atrasados(int id_usuario);
int contar_prestamos_atrasados(int id_usuario);

#endif
