#include "server.hpp"
#include "sala.hpp"
#include "cliente.hpp"

using namespace servidor;

int main() {  
  servidor::Servidor servidor(1234);
  servidor.crea_conexion();
  servidor.escucha();
  servidor.cierra_socket(0);
  return 0;
}
