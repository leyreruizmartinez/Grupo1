#ifndef CLIENTE_H
#define CLIENTE_H

#include <string>
#include <unordered_map>
extern std::string SERVER_IP;
extern int SERVER_PORT;
extern std::ofstream logFile;


extern std::unordered_map<std::string, std::string> cache;

void leerConfiguracion(const std::string& archivo);
std::string enviarComando(const std::string& comando);
void menuInicio();
void menuPrincipal(const std::string& correo);

#endif
