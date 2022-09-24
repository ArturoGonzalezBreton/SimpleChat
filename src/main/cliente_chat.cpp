#include "cliente.hpp"
#include <iostream>
#include <string>

using namespace std;
using namespace cliente;

int main() {
  string id;
  
  cliente::Cliente cliente(1234, "127.0.0.1");
  cliente.crea_conexion();
  cliente.conecta();
  
  string mensaje;
  string respuesta;
  while(mensaje != "DISCONNECT") {
    if (cliente.get_id().empty()) {
      do {
	cout << "Ingrese un identificador" << endl;
	cin >> id;
	cliente.set_id(id);
	mensaje = "{ \"type\": \"IDENTIFY\", \n";
	mensaje += "\"username\" : \"";
	mensaje.append(id).append("\" }");
	cliente.envia_mensaje(mensaje);
	respuesta = cliente.recibe_mensajes();
	cout << "s> " << respuesta << endl;
      } while (respuesta.find("WARNING") != std::string::npos);
    }
    cout << "> ";
    getline(cin, mensaje);
    cliente.envia_mensaje(mensaje);
    respuesta = cliente.recibe_mensajes();
    cout << "s> " << respuesta << endl;
  }
  cliente.desconecta();
  return 0;
}
