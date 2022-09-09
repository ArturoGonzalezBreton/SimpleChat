#include "cliente.hpp"
#include <iostream>
#include <string>

using namespace std;
using namespace cliente;

int main() {
  string id;
  cout << "Ingrese un identificador" << endl;
  cin >> id;
  cliente::Cliente cliente(1234, "127.0.0.1", id);
  cliente.crea_conexion();
  cliente.conecta();
  cliente.envia_mensajes();
  cliente.desconecta();
  return 0;
}
