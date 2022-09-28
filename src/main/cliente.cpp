#include "estado.hpp"
#include "mensaje.hpp"
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
#include <json/json.h>
  
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
void Cliente::envia_mensaje(std::string mensaje) {
  if (send(conexion, mensaje.c_str(), mensaje.size(), 0) == -1) {	  
    cout << "El mensaje no pudo ser enviado al servidor" << endl;
  }
}

/*
 * Recibe mensajes del servidor.
 */
string Cliente::recibe_mensajes() {
  char buffer[4096];
  memset(buffer, 0, 4096);
  int msj_recibido = recv(conexion, buffer, 4096, 0);
  if (msj_recibido == -1)
    throw runtime_error("Ocurrió un error al recibir el mensaje");
  if (msj_recibido == 0)
    throw runtime_error("El servidor se ha desconectado");
  
  return string(buffer, 0, msj_recibido); 
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
 * Devuelve el identificador del usuario.
 */
void Cliente::set_id(std::string id) {
  (this -> usuario).set_nombre(id);
}

/*
 * Devuelve el usuario.
 */
usuario::Usuario Cliente::get_usuario() {
  return this -> usuario;
}

/*
 * Devuelve el estado del cliente.
 */
estado::Estado Cliente::get_estado() {
  return this -> estado;
}

/*
 * Asigna estado al cliente.
 */
void Cliente::set_estado(estado::Estado estado) {
  this -> estado = estado;
}

/*
 * Nos dice si son iguales.
 */
bool Cliente::operator==(const Cliente& otro) const {
  return conexion == otro.conexion and
    puerto == otro.puerto and
    direccion_ip == otro.direccion_ip and
    usuario == otro.usuario and
    estado == otro.estado;
}

/*
 * Nos dice si son diferentes.
 */
bool Cliente::operator!=(const Cliente& otro) const {
  return !(conexion == otro.conexion and
	   puerto == otro.puerto and
	   direccion_ip == otro.direccion_ip and
	   usuario == otro.usuario and
	   estado == otro.estado);
}
