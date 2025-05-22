#ifndef SERVIDOR_H
#define SERVIDOR_H

#define PUERTO_SERVIDOR 8080

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#include "servidor.h"
#include "protocolo.h"
#include "log.h"
#include "../src/bd.h"
#include "../src/registro.h"

void iniciar_servidor();

#endif
