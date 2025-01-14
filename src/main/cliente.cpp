#include "cliente.hpp"
#include <json/json.h>
  
using namespace std;

/*
 * Constructor que recibe únicamente el puerto.
 */
cliente::Cliente::Cliente(int puerto) {
  this -> puerto = puerto;
}

/*
 * Constructor que recibe el puerto y la direccion ip.
 */
cliente::Cliente::Cliente(int puerto, std::string direccion_ip) {
  this -> puerto = puerto;
  this -> direccion_ip = direccion_ip;
}

/*
 * Constructor que recibe el puerto al que se conectará, la dirección ip
 * del servidor y el identificador del usuario del cliente.
 */   
cliente::Cliente::Cliente(int puerto, std::string direccion_ip, std::string id) {
  this -> puerto = puerto;
  this -> direccion_ip = direccion_ip;
  (this -> usuario).set_nombre(id);  
}

/*
 * Crea un socket y lo une al puerto en el que el servidor
 * escuchará conexiones.
 */
int cliente::Cliente::crea_conexion() {
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
void cliente::Cliente::conecta() {
  if (connect(conexion, (sockaddr*)&enchufe, sizeof(enchufe)) == -1) {
    throw runtime_error("No se pudo conectar al servidor");
  }
}

/*
 * Envía mensajes al servidor.
 */
void cliente::Cliente::envia_mensaje(std::string mensaje) {
  if (send(conexion, mensaje.c_str(), mensaje.size(), 0) == -1) {	  
    throw runtime_error("Ocurrió un error al recibir el mensaje");
  }
}

/*
 * Recibe mensajes del servidor.
 */
string cliente::Cliente::recibe_mensajes() {
  char buffer[4096];
  memset(buffer, 0, 4096);
  int msj_recibido = recv(conexion, buffer, 4096, 0);
  if (msj_recibido == -1)
    throw runtime_error("Ocurrió un error al recibir el mensaje");
  if (msj_recibido == 0)
    throw runtime_error("Se ha desconectado del servidor");
  
  return string(buffer, 0, msj_recibido); 
}

/*
 * Desconecta al cliente.
 */

void cliente::Cliente::desconecta() {
  close(this -> conexion);
}

/*
 * Devuelve la conexión del lado del cliente.
 */
int cliente::Cliente::get_conexion() {
  return this -> conexion;
}

/*
 * Asigna la conexion del lado del cliente.
 */
void cliente::Cliente::set_conexion(int conexion) {
  this -> conexion = conexion;
}

/*
 * Devuelve el socket del cliente.
 */
sockaddr_in cliente::Cliente::get_socket() {
  return this -> enchufe;
}

/*
 * Asigna un enchufe al cliente.
 */
void cliente::Cliente::set_socket(sockaddr_in enchufe) {
  this -> enchufe = enchufe;
}

/*
 * Devuelve el identificador del usuario.
 */
std::string cliente::Cliente::get_id() {
  return (this -> usuario).get_nombre();
}

/*
 * Devuelve el identificador del usuario.
 */
void cliente::Cliente::set_id(std::string id) {
  (this -> usuario).set_nombre(id);
}

/*
 * Devuelve el usuario.
 */
usuario::Usuario cliente::Cliente::get_usuario() {
  return this -> usuario;
}

/*
 * Devuelve el estado del cliente.
 */
estado::Estado cliente::Cliente::get_estado() {
  return this -> estado;
}

/*
 * Asigna estado al cliente.
 */
void cliente::Cliente::set_estado(estado::Estado estado) {
  this -> estado = estado;
}

/*
 * Nos dice si son iguales.
 */
bool cliente::Cliente::operator==(const cliente::Cliente& otro) const {
  return conexion == otro.conexion and
    puerto == otro.puerto and
    direccion_ip == otro.direccion_ip and
    usuario == otro.usuario and
    estado == otro.estado;
}

/*
 * Nos dice si son diferentes.
 */
bool cliente::Cliente::operator!=(const cliente::Cliente& otro) const {
  return !(conexion == otro.conexion and
	   puerto == otro.puerto and
	   direccion_ip == otro.direccion_ip and
	   usuario == otro.usuario and
	   estado == otro.estado);
}

/*
 * Asigna ip del servidor a conectarse.
 */
void cliente::Cliente::set_ip(string ip) {
  this -> direccion_ip = ip;
}

/*
 * Devuelve ip del servidor a conectarse.
 */
string cliente::Cliente::get_ip() {
  return this -> direccion_ip;
}

/*
 * Asigna puerto del servidor a conectarse.
 */
void cliente::Cliente::set_puerto(int puerto) {
  this -> puerto = puerto;
}

/*
 * Devuelve puerto del servidor a conectarse.
 */
int cliente::Cliente::get_puerto() {
  return this -> puerto;
}
