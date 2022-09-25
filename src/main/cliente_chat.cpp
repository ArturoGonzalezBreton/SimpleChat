#include "cliente.hpp"
#include <iostream>
#include <string>
#include <thread>

using namespace std;
using namespace cliente;

cliente::Cliente client(1234, "127.0.0.1");

void impr_recv();
void impr_env();

void impr_recv() {
  string respuesta;
  while(true) {
    try {
      respuesta = client.recibe_mensajes();
      cout << "s> " << respuesta << endl;
    } catch (std::runtime_error& e) {
      cout << e.what() << endl;
      break;
    }
  }
}

void impr_env() {
  string mensaje;
  while (mensaje.find("DISCONNECT") != std::string::npos) {
    cout << "> ";
    cin >> mensaje;
    client.envia_mensaje(mensaje);
  }
}

int main() {
  string id;
  
  //cliente::Cliente cliente(1234, "127.0.0.1");
  client.crea_conexion();
  client.conecta();
  
  string mensaje;
  string respuesta;
  //while(mensaje.find("DISCONNECT") != std::string::npos) {
  if (client.get_id().empty()) {
    do {
      cout << "Ingrese un identificador" << endl;
      cin >> id;
      client.set_id(id);
      mensaje = "{ \"type\": \"IDENTIFY\", \n";
      mensaje += "\"username\" : \"";
      mensaje.append(id).append("\" }");
      client.envia_mensaje(mensaje);
      respuesta = client.recibe_mensajes();
      cout << "s> " << respuesta << endl;
    } while (respuesta.find("WARNING") != std::string::npos);
  }
  thread hilo(impr_env);
  thread hilo1(impr_recv);
  hilo.join();
  hilo1.join();
  //cout << "> ";
  // cin >> mensaje;
  //   cliente.envia_mensaje(mensaje);
  //   respuesta = cliente.recibe_mensajes();
  //   cout << "s> " << respuesta << endl;
  // }
  client.desconecta();
  return 0;
}

