#include "usuario.hpp"
#include "cliente.hpp"
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
  
using namespace std;
using namespace cliente;

/*
 * Crea un socket y lo une al puerto en el que el servidor
 * escuchará conexiones.
 */
int Cliente::crea_conexion() {
  this -> conexion = socket(AF_INET, SOCK_STREAM, 0);
  if (this -> conexion == -1) {
    throw runtime_error("Ocurrió un error con la conexión");
  }
  enchufe.sin_family = AF_INET;
  enchufe.sin_port = htons(puerto);
  inet_pton(AF_INET, direccion_ip.c_str(), &enchufe.sin_addr);
  return 0;
    }

/*
 * Se conecta al servidor.
 */
void Cliente::conecta() {
  if (connect(conexion, (sockaddr*)&enchufe, sizeof(enchufe)) == -1) {
	throw runtime_error("No se pudo conectar al servidor");
  }
}

/*
 * Envía mensajes al servidor.
 */
void Cliente::envia_mensajes() {
  char buffer[4096];
  string mensaje;
  do {      
    cout << "> ";
    getline(cin, mensaje);
    if (mensaje == "\\DESCONECTA") {
	  break;
	  desconecta();
    }
    if (send(conexion, mensaje.c_str(), mensaje.size() + 1, 0) == -1) {	  
      cout << "El mensaje no pudo ser enviado al servidor" << endl;
      continue;
    }
    memset(buffer, 0, 4096);
    int respuesta = recv(conexion, buffer, 4096, 0);
    if (respuesta == -1) {
      cout << "Ocurrió un error al recibir el mensaje del servidor" << endl;
        } else {
      cout << "SERVER> " << string(buffer, respuesta) << "\r\n";
        }
  } while(true);
    }

/*
 * Desconecta al cliente.
 */

void Cliente::desconecta() {
  close(this -> conexion);
}

/*
 * Devuelve la conexión del lado del cliente.
 */
int Cliente::get_conexion() {
  return this -> conexion;
}

/*
 * Asigna la conexion del lado del cliente.
 */
void Cliente::set_conexion(int conexion) {
  this -> conexion = conexion;
}

/*
 * Devuelve el socket del cliente.
 */
sockaddr_in Cliente::get_socket() {
  return this -> enchufe;
}

/*
 * Asigna un enchufe al cliente.
 */
void Cliente::set_socket(sockaddr_in enchufe) {
  this -> enchufe = enchufe;
}

/*
 * Devuelve el identificador del usuario.
 */
std::string Cliente::get_id() {
  return (this -> usuario).get_nombre();
}

/*
 * Devuelve el usuario.
 */
usuario::Usuario Cliente::get_usuario() {
  return this -> usuario;
}
