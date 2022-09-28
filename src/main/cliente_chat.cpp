#include "cliente.hpp"
#include <iostream>
#include <string>
#include <thread>

using namespace std;
//using namespace cliente;

void impr_recv(cliente::Cliente &client);
void impr_env(cliente::Cliente &client);

void impr_recv(cliente::Cliente &client) {
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

void impr_env(cliente::Cliente &client) {
  string mensaje;
  while (mensaje.find("DISCONNECT") == std::string::npos) {
    cin >> mensaje;
    client.envia_mensaje(mensaje);
  }
}

int main() {
  string id;
  string ip;
  string puerto;
  
  cout << "Escribe la direccion ip:" << endl;
  cin >> ip;
  cout << "Escribe el puerto:" << endl;
  cin >> puerto;

  int port;

  try {
    port = stoi(puerto);
  } catch (std::invalid_argument &ia) {
    cout << "Puerto inválido" << endl;
    return 0;
  }
  
  cliente::Cliente client(port, ip);
  client.crea_conexion();

  try {
    client.conecta();
  } catch (std::runtime_error& e) {
    cout << e.what() << endl;
    return 0;
  }
  
  string mensaje;
  string respuesta;

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
  thread hilo(impr_env, ref(client));
  impr_recv(client);
  hilo.join();
  client.desconecta();
  return 0;
}

