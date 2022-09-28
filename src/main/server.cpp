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
    
    hilos.push_back(thread(&Servidor::recibe_mensaje, this, ref(cliente)));
  }
  for (auto& it : hilos) {
    it.join();
  }
}

/*
 * Asigna identificador al cliente.
 */
void servidor::Servidor::identifica_cliente(Cliente &cliente, string id) {
  string respuesta;
  if (clientes.find(id) != clientes.end()) {
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

    for (auto& [key, val] : clientes) {
      send((val.get_conexion()), aviso.c_str(), aviso.size() + 1 , 0);
    }
  }
  cliente.set_id(id);
  this -> clientes.insert({cliente.get_id(), cliente});
  send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);  
}

/*
 * Asigna un estado al cliente.
 */
void servidor::Servidor::asigna_estado(Cliente &cliente, estado::Estado estado) {
  string respuesta;
  if (cliente.get_estado() == estado) {
    respuesta = "{ \"type\": \"WARNING\", \n";						     
    respuesta += " \"message\": \"El estado ya es '";
    respuesta.append(estado::a_string(estado)).append("'\", \n");
    respuesta += " \"operation\": \"STATUS\", \n";
    respuesta += " \"status\": \"";
    respuesta.append(estado::a_string(estado)).append("\" }");
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
    aviso.append(estado::a_string(estado)).append("\" }");

    for (auto& [key, val] : clientes) {
      if (key != cliente.get_id()) 
      send(val.get_conexion(), aviso.c_str(), aviso.size() + 1 , 0);
    }
  }
  cliente.set_estado(estado);
  send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
}

/*
 * Envia la lista de usuarios a un cliente.
 */
void servidor::Servidor::envia_usuarios(Cliente &cliente) {
  string respuesta;
  string usuarios;
  usuarios = "[ ";
  for (auto& [key, val] : clientes) {
    usuarios += "\"";
    usuarios.append(key);
    usuarios += "\"";
    usuarios += ",";
  }
  usuarios.erase(usuarios.end()-1);
  usuarios += " ]";
  respuesta = "{ \"type\": \"USER_LIST\", \n";
  respuesta += " \"usernames\": ";
  respuesta.append(usuarios).append(" }");
  send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);  
}

/*
 * Envía un mensaje privado.
 */
void servidor::Servidor::envia_mensaje_privado(Cliente &remitente, string id_destinatario, string mensaje) {
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
void servidor::Servidor::envia_mensaje_publico(Cliente &remitente, string mensaje) {
  string respuesta;
  respuesta = "{ \"type\": \"PUBLIC_MESSAGE_FROM\", \n";
  respuesta += "{ \"username\": \"";
  respuesta.append(remitente.get_id()).append("\", \n");
  cout << respuesta << endl;
  respuesta += " \"message\": \"";
  respuesta.append(mensaje).append("\" }");

  for (auto& [key, val] : clientes) {
    if (key != remitente.get_id()) {
      send(val.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
    }
  }
}

/*
 * Crea una nueva sala.
 */
void servidor::Servidor::crea_sala(Cliente &creador, sala::Sala &sala) {
  string respuesta;
  if (this -> salas.find(sala.get_id()) != salas.end()) {
    respuesta = "{ \"type\": \"WARNING\", \n";
    respuesta += " \"message\": \"El cuarto '";
    respuesta.append(sala.get_id()).append("' ya existe\", \n");
    respuesta += " \"operation\": \"NEW_ROOM\", \n";
    respuesta += " \"roomname\": \"";
    respuesta.append(sala.get_id()).append("\" }");
    send(creador.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  } else {
    //sala::Sala nueva_sala = sala::Sala(creador, id_sala);
    this -> salas.insert({sala.get_id(), sala});
    respuesta = "{ \"type\": \"INFO\", \n";
    respuesta += " \"message\": \"success\", \n";
    respuesta += " \"operation\": \"NEW_ROOM\" }";
    send(creador.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
    //sala.agrega_miembro(creador);
  }
}

/*
 * Manda invitación a un usuario para unirse a una sala.
 * Regresa false en caso de error.
 */
bool servidor::Servidor::invita_a_sala(Cliente &remitente, string id_invitado, string id_sala) {
  string respuesta;
  if (!salas[id_sala].es_miembro(remitente)) {
    return false;
  } else {
    respuesta = "{ \"type\": \"INFO\", \n";
    respuesta += " \"message\": \"success\"";
    respuesta += " \"operation\": \"INVITE\", \n";
    respuesta += " \"roomname\": \"";
    respuesta.append(id_sala).append("\" }");
    
    string invitacion;
    invitacion = "{ \"type\": \"INVITATION\", \n";
    invitacion += " \"message\": \"";
    invitacion.append(remitente.get_id());
    invitacion += " te invita al cuarto '";
    invitacion.append(id_sala).append("\", \n");
    invitacion += " \"username\": \"";
    invitacion.append(remitente.get_id()).append("\", \n");
    invitacion += " \"roomname\": \"";
    invitacion.append(id_sala).append("\" }");
    salas[id_sala].agrega_invitado(clientes[id_invitado].get_usuario());
    send(remitente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
    send(clientes[id_invitado].get_conexion(), invitacion.c_str(), invitacion.size() + 1, 0);
    return true;
  }
}

/*
 * Envía un mensaje al cliente indicando que el usuario no existe.
 */
void servidor::Servidor::envia_error_usuario(Cliente &cliente, string no_existente) {
  string respuesta;
  respuesta = "{ \"type\": \"WARNING\", \n";
  respuesta += " \"message\": \"El usuario '";
  respuesta.append(no_existente).append("' no existe\", \n");
  respuesta += " \"operation\": \"INVITE\", \n";
  respuesta += " \"username\": \"";
  respuesta.append(no_existente).append("\" }");
  send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
}

/*
 * Agrega a un usuario a la sala.
 */
void servidor::Servidor::agrega_a_sala(cliente::Cliente &cliente, sala::Sala &sala) {
  string respuesta;
  if (!sala.hay_invitacion(cliente.get_usuario()) and sala.get_creador() != cliente) {
    respuesta = "{ \"type\": \"WARNING\", \n";
    respuesta += " \"message\": \"El usuario no ha sido invitado al cuarto '";
    respuesta.append(sala.get_id()).append("'\", \n");
    respuesta += " \"operation\": \"JOIN_ROOM\", \n";
    respuesta += " \"roomname\": \"";
    respuesta.append(sala.get_id()).append("\" }");
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  } else if (sala.es_miembro(cliente)) {
    respuesta = "{ \"type\": \"WARNING\", \n";
    respuesta += " \"message\": \"El usuario ya se unió al cuarto '";
    respuesta.append(sala.get_id()).append("'\", \n");
    respuesta += " \"operation\": \"JOIN_ROOM\", \n";
    respuesta += " \"roomname\": \"";
    respuesta.append(sala.get_id()).append("\" }");
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  } else {
    string aviso;
    if (sala.agrega_miembro(cliente)) {
      respuesta = "{ \"type\": \"INFO\", \n";
      respuesta += " \"message\": \"success\"";
      respuesta += " \"operation\": \"JOIN_ROOM\", \n";
      respuesta += " \"roomname\": \"";
      respuesta.append(sala.get_id()).append("\" }");
      send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
      aviso = "{ \"type\": \"JOINED_ROOM\", \n";
      aviso += "{ \"roomname\": \"";
      aviso.append(sala.get_id()).append("\", \n");
      aviso += " \"username\": \"";
      aviso.append(cliente.get_id()).append("\" }");
      for (auto& [key, val] : sala.get_miembros()) {
	if (val != cliente) 
	  send(val.get_conexion(), aviso.c_str(), aviso.size() + 1 , 0);      
      }
    }
  }
}

/*
 * Envía los miembros de una sala a un cliente.
 */
void servidor::Servidor::envia_miembros(cliente::Cliente &cliente, std::string id_sala) {
  string respuesta;
  if (!salas[id_sala].es_miembro(cliente)) {
    respuesta = "{ \"type\": \"WARNING\", \n";
    respuesta += " \"message\": \"El usuario no se ha unido al cuarto '";
    respuesta.append(id_sala).append("'\", \n");
    respuesta += " \"operation\": \"ROOM_USERS\", \n";
    respuesta += " \"roomname\": \"";
    respuesta.append(id_sala).append("\" }");
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  } else {
    string miembros;
    miembros = "[ ";
    for (auto& [key, val] : salas[id_sala].get_miembros()) {
      miembros += "\"";
      miembros.append(key);
      miembros += "\"";
      miembros += ",";	
    }
    miembros.erase(miembros.end()-1);
    miembros += " ]";
    respuesta = "{ \"type\": \"ROOM_USER_LIST\", \n";
    respuesta += " \"usernames\": ";
    respuesta.append(miembros).append(" }");
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  }
}

/*
 * Envía un mensaje a los usuarios de una sala.
 */
void servidor::Servidor::envia_mensaje_sala(Cliente &cliente, sala::Sala &sala, std::string mensaje) {
  string respuesta;
  string id_sala = sala.get_id();
  if (!salas[id_sala].hay_invitacion(cliente.get_usuario()) and !salas[id_sala].es_miembro(cliente)) {
    respuesta = "{ \"type\": \"WARNING\", \n";
    respuesta += " \"message\": \"El usuario no se ha unido al cuarto '";
    respuesta.append(id_sala).append("'\", \n");
    respuesta += " \"operation\": \"\", \n";
    respuesta += " \"roomname\": \"";
    respuesta.append(id_sala).append("\" }");
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  } else {
    respuesta = "{ \"type\": \"ROOM_MESSAGE_FROM\", \n";
    respuesta += "{ \"roomname\": \"";
    respuesta.append(id_sala).append("\", \n");
    respuesta += "{ \"username\": \"";
    respuesta.append(cliente.get_id()).append("\", \n");
    respuesta += " \"message\": \"";
    respuesta.append(mensaje).append("\" }");
    for (auto& [key, val] : sala.get_miembros()) {
      if (val != cliente)
	send(val.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
    }
  }
}

/*
 * Saca a un cliente de una sala.
 */
void servidor::Servidor::saca_de_sala(Cliente &cliente, sala::Sala &sala) {
  string respuesta;
  string id_sala = sala.get_id();
  if ((!sala.hay_invitacion(cliente.get_usuario()) and cliente != sala.get_creador()) ||
      !sala.es_miembro(cliente)) {
    respuesta = "{ \"type\": \"WARNING\", \n";
    respuesta += " \"message\": \"El usuario no se ha unido al cuarto '";
    respuesta.append(id_sala).append("'\", \n");
    respuesta += " \"operation\": \"LEAVE_ROOM\", \n";
    respuesta += " \"roomname\": \"";
    respuesta.append(id_sala).append("\" }");
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  } else {
    string aviso;
    respuesta = "{ \"type\": \"INFO\", \n";
    respuesta += " \"message\": \"success\"";
    respuesta += " \"operation\": \"LEAVE_ROOM\", \n";
    respuesta += " \"roomname\": \"";
    respuesta.append(id_sala).append("\" }");
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
    aviso = "{ \"type\": \"LEFT_ROOM\", \n";
    aviso += "{ \"roomname\": \"";
    aviso.append(id_sala).append("\", \n");
    aviso += " \"username\": \"";
    aviso.append(cliente.get_id()).append("\" }");    
    sala.elimina_miembro(cliente);
    for (auto& [key, val] : sala.get_miembros()) {
      send(val.get_conexion(), aviso.c_str(), aviso.size() + 1 , 0);
    }
    if (sala.es_vacia())
      salas.erase(sala.get_id());    
  }
}

/*
 * Notifica a los usuarios que alguien se desconectó.
 */
void servidor::Servidor::notifica_salida(Cliente &cliente) {
  string aviso;
  aviso = "{ \"type\": \"DISCONNECTED\", \n";
  aviso += " \"username\": \"";
  aviso.append(cliente.get_id()).append("\" }");
  map<string, Cliente>::iterator it;
  for (it = clientes.begin(); it != clientes.end(); it++)
    send((it -> second).get_conexion(), aviso.c_str(), aviso.size() + 1 , 0);

  map<string, sala::Sala>::iterator it_salas;
  for (it_salas = salas.begin(); it_salas != salas.end(); it_salas++) {
    sala::Sala sala = it_salas -> second;
    map<string, Cliente>::iterator it_client;
    string id_sala = sala.get_id();
    
    aviso = "{ \"type\": \"LEFT_ROOM\", \n";
    aviso += "{ \"roomname\": \"";
    aviso.append(id_sala).append("\", \n");
    aviso += " \"username\": \"";
    aviso.append(cliente.get_id()).append("\" }");
    
    sala.elimina_miembro(cliente);

    for (it_client = sala.get_miembros().begin(); it_client != clientes.end(); it_client++) 
      send((it_client -> second).get_conexion(), aviso.c_str(), aviso.size() + 1 , 0);
  }

  cierra_cliente(cliente.get_id());
}

/*
 * Cierra la conexion.
 */
void servidor::Servidor::cierra_socket() {
  for (auto& [key, val] : clientes)
    cierra_cliente(val.get_id());
  close(conexion);
}

/*
 * Recibe mensajes y decide a qué método llamar dependiendo
 * del mensaje recibido.
 */
void servidor::Servidor::sirve(Cliente cliente) {
  bool recibido = true;
  while (recibido) 
    recibido = recibe_mensaje(cliente);
}

/*
 * Cierra la conexion del cliente;
 */
void servidor::Servidor::cierra_cliente(string id) {
  close(clientes[id].get_conexion());
  clientes.erase(id);
}

bool servidor::Servidor::recibe_mensaje(cliente::Cliente cliente) {
  char buffer[4096];

  while (true) {
    memset(buffer, 0, 4096);
    int msj_recibido = recv(cliente.get_conexion(), buffer, 4096, 0);
    
    if (msj_recibido == -1) {
      throw runtime_error("Ocurrió un error al recibir el mensaje.");
    }
    
    if (msj_recibido == 0){
      cout << "Cliente desconectado" << endl;
      cierra_cliente(cliente.get_id());
      return false;
    }
    
    string mensaje = string(buffer, 0, msj_recibido);
    
    cout << mensaje << endl;
    
    Reader reader;
    Value msj_json;

    
    reader.parse(mensaje, msj_json);

    if (!msj_json.isMember("type")) {
      cierra_cliente(cliente.get_id());
      cout << "No es un json valido" << endl;
      return false;
    }
    
    switch (mensaje::tabla[msj_json["type"].asString()]) {
    
    case mensaje::Mensaje::IDENTIFY:
      if (!msj_json.isMember("username")) {
	cierra_cliente(cliente.get_id());
	cout << "IDENTIFY no válido" << endl;
	return false;
      }
      identifica_cliente(cliente, msj_json["username"].asString());
      break;
      
    case mensaje::Mensaje::STATUS:
      if (!msj_json.isMember("status")) {
	cierra_cliente(cliente.get_id());
	cout << "STATUS no válido" << endl;
	return false;
      }
      switch (estado::tabla[msj_json["status"].asString()]) {
	
      case estado::AWAY:
	asigna_estado(cliente, estado::Estado::AWAY);
	break;
	
      case estado::ACTIVE:
	asigna_estado(cliente, estado::Estado::ACTIVE);
	break;
      
      case estado::BUSY:
	asigna_estado(cliente, estado::Estado::BUSY);
	break;
	
      default:
	cierra_cliente(cliente.get_id());
	cout << "Estado no válido" << endl;
	return false;
      }
      break;    
      
    case mensaje::Mensaje::USERS:
      envia_usuarios(cliente);
      break;
      
    case mensaje::Mensaje::MESSAGE:
      if (!msj_json.isMember("username") || !msj_json.isMember("message")) {
	cierra_cliente(cliente.get_id());
	cout << "MESSAGE no válido" << endl;
	return false;
      }
      envia_mensaje_privado(cliente, msj_json["username"].asString(), msj_json["message"].asString());
      break;
      
    case mensaje::Mensaje::PUBLIC_MESSAGE:
      if (!msj_json.isMember("message")) {
	cierra_cliente(cliente.get_id());
	cout << "PUBLIC_MESSAGE no válido" << endl;
	return false;
      }
      envia_mensaje_publico(cliente, msj_json["message"].asString());
      break;
      
    case mensaje::Mensaje::NEW_ROOM: {
      if (!msj_json.isMember("roomname")) {
	cierra_cliente(cliente.get_id());
	cout << "NEW_ROOM no válido" << endl;
	return false;
      }
      sala::Sala sala(cliente, msj_json["roomname"].asString());
      crea_sala(cliente, sala);
      break;
    }
      
    case mensaje::Mensaje::INVITE: {
      if (!msj_json.isMember("roomname") || !msj_json.isMember("usernames")) {
	cierra_cliente(cliente.get_id());
	cout << "INVITE no válido" << endl;
	return false;
      }
      Value invitados = msj_json["usernames"];
      if (invitados.empty()) {
	cierra_cliente(cliente.get_id());
	cout << "usernames en INVITE no válido" << endl;
	return false;
      }
      
      bool existen_usuarios = true;
      for (uint i = 0; i < invitados.size() and existen_usuarios; i++) {
	existen_usuarios = existen_usuarios and (clientes.find(invitados[i].asString()) != clientes.end());
	if (existen_usuarios == false) {	  
	  envia_error_usuario(cliente, invitados[i].asString());
	}
      }

      if (!existe_sala(msj_json["roomname"].asString())) {
	envia_error_sala(cliente, msj_json["roomname"].asString(), mensaje::a_string(mensaje::Mensaje::INVITE));
	break;
      }
	
      
      if (existen_usuarios) {
	bool existe_sala = true;
	for (uint i = 0; i < invitados.size() and existe_sala; i++) {
	  existe_sala = invita_a_sala(cliente, invitados[i].asString(), msj_json["roomname"].asString());
	}
      }
      break;
    }
      
    case mensaje::Mensaje::JOIN_ROOM:
      if (!msj_json.isMember("roomname")) {
	cierra_cliente(cliente.get_id());
	cout << "JOIN_ROOM no válido" << endl;
	return false;
      }
      if (!existe_sala(msj_json["roomname"].asString())) {
	envia_error_sala(cliente, msj_json["roomname"].asString(),
			 mensaje::a_string(mensaje::Mensaje::JOIN_ROOM));
      } else {
	agrega_a_sala(cliente, salas[msj_json["roomname"].asString()]);
      }  
      break;
      
    case mensaje::Mensaje::ROOM_USERS:
      if (!msj_json.isMember("roomname")) {
	cierra_cliente(cliente.get_id());
	cout << "ROOM_USERS no válido" << endl;
	return false;
      }
      if (!existe_sala(msj_json["roomname"].asString())) {
	envia_error_sala(cliente, msj_json["roomname"].asString(),
			 mensaje::a_string(mensaje::Mensaje::ROOM_USERS));
      } else {
	envia_miembros(cliente, msj_json["roomname"].asString());
      }
      break;
      
    case mensaje::Mensaje::ROOM_MESSAGE: 
      if (!msj_json.isMember("roomname") || !msj_json.isMember("message")) {
	cierra_cliente(cliente.get_id());
	cout << "ROOM_MESSAGE no válido" << endl;
	return false;
      }
      if (!existe_sala(msj_json["roomname"].asString())) {
	envia_error_sala(cliente, msj_json["roomname"].asString(),
			 mensaje::a_string(mensaje::Mensaje::ROOM_MESSAGE));
      } else {
	envia_mensaje_sala(cliente, salas[msj_json["roomname"].asString()], msj_json["message"].asString());
      }
      break;

    case mensaje::Mensaje::LEAVE_ROOM:
      if (!msj_json.isMember("roomname")) {
	cierra_cliente(cliente.get_id());
	cout << "LEAVE_ROOM no válido" << endl;
	return false;
      }
      if (!existe_sala(msj_json["roomname"].asString())) {
	envia_error_sala(cliente, msj_json["roomname"].asString(),
			 mensaje::a_string(mensaje::Mensaje::LEAVE_ROOM));
      } else {
	saca_de_sala(cliente, salas[msj_json["roomname"].asString()]);
      }
      break;
      
    case mensaje::Mensaje::DISCONNECT:
      notifica_salida(cliente);
      
    default:
      return false;    
    }
  }
  return true;
}
 
/*
 * Verifica si existe una sala.
 */
bool servidor::Servidor::existe_sala(string id_sala) {
  return salas.find(id_sala) != salas.end();
}


/*
 * Envía un mensaje al cliente indicando que la sala no existe.
 */
 void servidor::Servidor::envia_error_sala(Cliente &cliente, string no_existente, string operacion) {
   string respuesta;
   respuesta = "{ \"type\": \"WARNING\", \n";
   respuesta += " \"message\": \"El cuarto '";
   respuesta.append(no_existente).append("' no existe\", \n");
   respuesta += " \"operation\": \"";
   respuesta.append(operacion).append("\", \n");
   respuesta += " \"roomname\": \"";
   respuesta.append(no_existente).append("\" }");
   send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
 }
