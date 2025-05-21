#include "cliente.h"
#include <fstream>

extern std::ofstream logFile;

int main() {
    leerConfiguracion("cliente_config.txt");
    logFile << "[INICIO CLIENTE] IP: " << SERVER_IP << ", PUERTO: " << SERVER_PORT << "\n";
    menuInicio();
    logFile.close();
    return 0;
}
