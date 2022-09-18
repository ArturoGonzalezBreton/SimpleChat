#include "estado.hpp"
#include "mensaje.hpp"
#include "server.hpp"
#include "sala.hpp"
#include "cliente.hpp"
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <list>
#include <map>
#include <thread>
#include <json/json.h>

using namespace std;
using namespace cliente;
using namespace Json;
using namespace estado;
    
/*
 * Crea un socket y lo une al puerto en el que escuchará
 * conexiones.
 */
void servidor::Servidor::crea_conexion() {
  this -> conexion = socket(AF_INET, SOCK_STREAM, 0);      
  if (conexion == -1) {
    throw runtime_error("Ocurrió un error al crear la conexión");
  }    
  enchufe.sin_family = AF_INET;
  enchufe.sin_port = htons(this -> puerto);
  inet_pton(AF_INET, "127.0.0.1", &(enchufe.sin_addr));
  
  bind(conexion, (sockaddr*)&(enchufe), sizeof(enchufe));
  
  listen(conexion, SOMAXCONN);      
}

/*
 * Escucha conexiones de los clientes.
 */
void servidor::Servidor::escucha() {
  while (true) {
    Cliente cliente(this -> puerto);
    sockaddr_in enchufe_cliente;
    socklen_t tam_cliente = sizeof(cliente.get_socket());
    cliente.set_conexion(accept(conexion, (sockaddr*)&enchufe_cliente,
				&tam_cliente));
    char nombre_cliente[NI_MAXHOST];
    char servicio[NI_MAXSERV];
    
    memset(nombre_cliente, 0, NI_MAXHOST); 
    memset(servicio, 0, NI_MAXSERV);
    getnameinfo((sockaddr*)&enchufe_cliente, sizeof(cliente), nombre_cliente,
		NI_MAXHOST, servicio, NI_MAXSERV, 0);

    inet_ntop(AF_INET, &enchufe_cliente.sin_addr, nombre_cliente, NI_MAXHOST);
    
    cliente.set_socket(enchufe_cliente);
    
    clientes.insert(pair<string, Cliente>(cliente.get_id(), cliente));

    sirve(clientes[cliente.get_id()]);
    // lanza hilo que recibe mensaje con el identificador del
    // cliente y se lo asigna.
    //thread thread(&Servidor::sirve, this, ref(cliente));
    //thread.join();
  }
}

/*
 * Asigna identificador al cliente.
 */
void servidor::Servidor::identifica_cliente(Cliente cliente, string id) {
  string respuesta;
  if (clientes.find(id) == clientes.end()) {
    respuesta = "{ \"type\": \"WARNING\", \n";						     
    respuesta += " \"message\": \"El usuario '";
    respuesta.append(id).append("' ya existe\", \n");
    respuesta += " \"operation\": \"IDENTIFY\", \n";
    respuesta += " \"username\": \"";
    respuesta.append(id).append("\" }");
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
    return;
  } else {
    respuesta = "{ \"type\": \"INFO\", \n"; 
    respuesta += " \"message\": \"success\", \n";
    respuesta += " \"operation\": \"IDENTIFY\" }";
    string aviso;
    aviso = "{ \"type\": \"NEW_USER\", \n";
    aviso += "{ \"username\": \"";
    aviso.append(id).append("\" }");

    for (map<string, Cliente>::iterator it = clientes.begin(); it != clientes.end(); it++) {
      send((it -> second).get_conexion(), aviso.c_str(), aviso.size() + 1 , 0);
    }
  }
  cliente.get_usuario().set_nombre(id);
  this -> clientes.insert({id, cliente});
  send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);  
}

/*
 * Asigna un estado al cliente.
 */
void servidor::Servidor::asigna_estado(Cliente cliente, Estado estado) {
  string respuesta;
  if (cliente.get_estado() == estado) {
    respuesta = "{ \"type\": \"WARNING\", \n";						     
    respuesta += " \"message\": \"El estado ya es '";
    respuesta.append(a_string(estado)).append("'\", \n");
    respuesta += " \"operation\": \"STATUS\", \n";
    respuesta += " \"status\": \"";
    respuesta.append(to_string(estado)).append("\" }");
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
    return;
  } else {
    respuesta = "{ \"type\": \"INFO\", \n"; 
    respuesta += " \"message\": \"success\", \n";
    respuesta += " \"operation\": \"STATUS\" }";
    string aviso;
    aviso = "{ \"type\": \"NEW_STATUS\", \n";
    aviso += "{ \"username\": \"";
    aviso.append(cliente.get_id()).append("\", \n");
    aviso += " \"status\": \"";
    aviso.append(to_string(estado)).append("\" }");

    for (map<string, Cliente>::iterator it = clientes.begin(); it != clientes.end(); it++) {
      send((it -> second).get_conexion(), aviso.c_str(), aviso.size() + 1 , 0);
    }
  }
  cliente.set_estado(estado);
  send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
}

/*
 * Envia la lista de usuarios a un cliente.
 */
void servidor::Servidor::envia_usuarios(Cliente cliente) {
  string respuesta;
  string usuarios;
  usuarios = "[ ";
  for (map<string, Cliente>::iterator it = clientes.begin(); it != clientes.end(); it++) {
    usuarios += "\"";
    usuarios.append(it -> first);
    usuarios += "\"";
    if (++it != clientes.end()) {
      usuarios += ",";
      it--;
    }
  }
  usuarios += " ]";
  respuesta = "{ \"type\": \"USER_LIST\", \n";
  respuesta += " \"usernames\": ";
  respuesta.append(usuarios).append(" }");
  send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);  
}

/*
 * Envía un mensaje privado.
 */
void servidor::Servidor::envia_mensaje_privado(Cliente remitente, string id_destinatario, string mensaje) {
  string respuesta;
  if (clientes.find(id_destinatario) == clientes.end()) {
    respuesta = "{ \"type\": \"WARNING\", \n";
    respuesta += " \"message\": \"El usuario '";
    respuesta.append(id_destinatario).append("' no existe\", \n");
    respuesta += " \"operation\": \"MESSAGE\", \n";
    respuesta += " \"username\": \"";
    respuesta.append(id_destinatario).append("\" }");
    send(remitente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0); 
  } else {
    respuesta = "{ \"type\": \"MESSAGE_FROM\", \n";
    respuesta += "{ \"username\": \"";
    respuesta.append(remitente.get_id()).append("\", \n");
    respuesta += " \"message\": \"";
    respuesta.append(mensaje).append("\" }");
    send(clientes[id_destinatario].get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  }
}

/*
 * Envía un mensaje público.
 */
void servidor::Servidor::envia_mensaje_publico(Cliente remitente, string mensaje) {
  string respuesta;
  respuesta = "{ \"type\": \"PUBLIC_MESSAGE_FROM\", \n";
  respuesta += "{ \"username\": \"";
  respuesta.append(remitente.get_id()).append("\", \n");
  respuesta += " \"status\": \"";
  respuesta.append(mensaje).append("\" }");
  
  for (map<string, Cliente>::iterator it = clientes.begin(); it != clientes.end(); it++) {
    if ((it -> second) != remitente) {
      send((it -> second).get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
    }
  }
}

/*
 * Crea una nueva sala.
 */
void servidor::Servidor::crea_sala(Cliente creador, string sala) {
  string respuesta;
  if (salas.find(sala) != salas.end()) {
    respuesta = "{ \"type\": \"WARNING\", \n";
    respuesta += " \"message\": \"El cuarto '";
    respuesta.append(sala).append("' ya existe\", \n");
    respuesta += " \"operation\": \"NEW_ROOM\", \n";
    respuesta += " \"roomname\": \"";
    respuesta.append(sala).append("\" }");
    send(creador.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  } else {
    sala::Sala nueva_sala(creador, sala);
    this -> salas.insert({sala, nueva_sala});
    respuesta = "{ \"type\": \"INFO\", \n";
    respuesta += "{ \"message\": \"success\", \n";
    respuesta += " \"operation\": \"NEW_ROOM\" }";
    send(creador.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  }
}

/*
 * Cierra la conexion.
 */
void servidor::Servidor::cierra_socket() {
  close(conexion);
}

/*
 * Recibe mensajes y decide a qué método llamar dependiendo
 * del mensaje recibido.
 */
void servidor::Servidor::sirve(Cliente cliente) {
  char buffer[4096];
  while (true)
    {
      memset(buffer, 0, 4096);
      
      int msj_recibido = recv(cliente.get_conexion(), buffer, 4096, 0);
      if (msj_recibido == -1){
	throw runtime_error("Ocurrió un error al recibir el mensaje.");
      }
      
      if (msj_recibido == 0){
	cout << "Cliente desconectado" << endl;
	break;
      }
      
      cout << string(buffer, 0, msj_recibido) << endl;
      send(cliente.get_conexion(), buffer, msj_recibido + 1, 0);
    }
}

/*
 * Cierra la conexion del cliente;
 */
void servidor::Servidor::cierra_cliente(string id) {
  close(clientes[id].get_conexion());
}

void servidor::Servidor::recibe_mensaje(cliente::Cliente cliente) {
  char buffer[4096];
  int msj_recibido = recv(cliente.get_conexion(), buffer, 4096, 0);
  memset(buffer, 0, 4096);
  string mensaje = string(buffer, 0, msj_recibido);

  Reader reader;
  Value msj_json;

  reader.parse(mensaje, msj_json);

  if (!msj_json.isMember("type")) {
    cierra_cliente(cliente.get_id());
  }
  
  switch (msj_json.get("type", "")) {
    
  case "IDENTIFY":
    if (!msj_json.isMember("username")) {
      cierra_cliente(cliente.get_id());
    }
    identifica_cliente(cliente, msj_json.get("username", ""));
    break;

  case "STATUS":
    if (!msj_json.isMember("status")) {
      cierra_cliente(cliente.get_id());
    }
    switch (msj_json.get("status", "")) {

    case "AWAY":
      asigna_estado(cliente, Estado::AWAY);
      break;

    case "ACTIVE":
      asigna_estado(cliente, Estado::ACTIVE);
      break;

    case "BUSY":
      asigna_estado(cliente, Estado::BUSY);
    }
    break;

  case "USERS":
    envia_usuarios(cliente);
    break;

  case "MESSAGE":
    if (!msj_json.isMember("username") || !msj_json.isMember("message")) {
      cierra_cliente(cliente.get_id());
    }
    envia_mensaje_privado(cliente, msj_json.get("username", ""), msj_json.get("message", ""));
    break;
    
  case "PUBLIC_MESSAGE":
    if (!msj_json.isMember("message")) {
      cierra_cliente(cliente.get_id());
    }
    envia_mensaje_publico(cliente, msj_json.get("message", ""));
    break;
    
  case "NEW_ROOM":
    if (!msj_json.isMember("roomname")) {
      cierra_cliente(cliente.get_id());
    }
    crea_sala(cliente, msj_json.get("rooomname", ""));
    break;
  }
}
