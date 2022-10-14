#include "server.hpp"
#include <json/json.h>

using namespace std;
using namespace cliente;

/*
 * Constructor que recibe el puerto en el que el servidor
 * Recibirá conexiones.
 */
servidor::Servidor::Servidor(int puerto) {
  this -> puerto = puerto;   
}

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
 * Crea la cadena Json que enviará a los clientes
 */
string servidor::Servidor::crea_respuesta_json(std::map<std::string, std::string> json) {
  string respuesta = "{ ";
  for (auto& [key, val] : json) {
    respuesta += "\"";
    respuesta += key;
    respuesta += "\":";
    if (val.find("[") == string::npos) {
      respuesta += "\"";
      respuesta += val;
      respuesta += "\",";
    } else {
      respuesta += val;
      respuesta += ",";
    }
  }
  respuesta.pop_back();
  respuesta += " }";
  return respuesta;
}

/*
 * Asigna identificador al cliente.
 */
void servidor::Servidor::identifica_cliente(Cliente &cliente, string id) {
  string respuesta;  
  if (clientes.find(id) != clientes.end()) {
    string mensaje = "El usuario ";
    mensaje += id;
    mensaje += " ya existe";
    respuesta = crea_respuesta_json({ {"type", "WARNING"},
				      {"message", mensaje},
				      {"operation", "IDENTIFY"},
				      {"username", id} });   
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  } else {
    respuesta = crea_respuesta_json({ {"type", "INFO"},
				      {"message", "succes"},
				      {"operation", "IDENTIFY"} });
    string aviso = crea_respuesta_json({ {"type", "NEW_USER"},
					 {"username", id} });    
    for (auto& [key, val] : clientes) {
      send((val.get_conexion()), aviso.c_str(), aviso.size() + 1 , 0);
    }
    cliente.set_id(id);
    cliente.set_estado(estado::Estado::ACTIVE);
    this -> clientes.insert({id, cliente});
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);  
  }
}

/*
 * Asigna un estado al cliente.
 */
void servidor::Servidor::asigna_estado(Cliente &cliente, estado::Estado estado) {
  string respuesta, status = estado::a_string(estado);
  if (cliente.get_estado() == estado) {
    string mensaje = "El estado ya es '";
    mensaje += status;
    mensaje += "'";
    respuesta = crea_respuesta_json({ {"type", "WARNING"},
				      {"message", mensaje},
				      {"operation", "STATUS"},
				      {"status", status} });   
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  } else {
    respuesta = crea_respuesta_json({ {"type", "INFO"},
				      {"message", "succes"},
				      {"operation", "STATUS"} });
    string aviso = crea_respuesta_json({ {"type", "NEW_STATUS"},
					 {"username", cliente.get_id()},
					 {"status", status} });    
    for (auto& [key, val] : clientes) {
      if (key != cliente.get_id()) 
      send(val.get_conexion(), aviso.c_str(), aviso.size() + 1 , 0);
    }
    cliente.set_estado(estado);
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  }
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
  usuarios.pop_back();
  usuarios += " ]";
  respuesta = crea_respuesta_json({ {"type", "USER_LIST"},
				    {"usernames", usuarios} });
  send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);  
}

/*
 * Envía un mensaje privado.
 */
void servidor::Servidor::envia_mensaje_privado(Cliente &remitente, string id_destinatario, string mensaje) {
  string respuesta = crea_respuesta_json({ {"type", "MESSAGE_FROM"},
					   {"username", remitente.get_id()},
					   {"message", mensaje} });
  send(clientes[id_destinatario].get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
}

/*
 * Envía un mensaje público.
 */
void servidor::Servidor::envia_mensaje_publico(Cliente &remitente, string mensaje) {
  string respuesta = crea_respuesta_json({ {"type", "PUBLIC_MESSAGE_FROM"},
					   {"username", remitente.get_id()},
					   {"message", mensaje} });
  for (auto& [key, val] : clientes) {
    if (key != remitente.get_id()) 
      send(val.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);   
  }
}

/*
 * Agrega una nueva sala.
 */
void servidor::Servidor::agrega_sala(Cliente &creador, sala::Sala &sala) {
  string respuesta;
  if (this -> salas.find(sala.get_id()) != salas.end()) {
    string mensaje = "El cuarto '";
    mensaje += sala.get_id();
    mensaje += "' ya esxiste";
    respuesta = crea_respuesta_json({ {"type", "WARNING"},
				      {"message", mensaje},
				      {"operation", "NEW_ROOM"},
				      {"roomname", sala.get_id()} });   
    send(creador.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  } else {
    respuesta = crea_respuesta_json({ {"type", "INFO"},
				      {"message", "succes"},
				      {"operation", "NEW_ROOM"} }); 
    this -> salas.insert({sala.get_id(), sala});
    send(creador.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  }
}

/*
 * Manda invitación a un usuario para unirse a una sala.
 * Regresa false en caso de error.
 */
bool servidor::Servidor::invita_a_sala(Cliente &remitente, string id_invitado, string id_sala) {
  string respuesta, invitacion;
  if (!salas[id_sala].es_miembro(remitente)) {
    return false;
  } else {
    respuesta = crea_respuesta_json({ {"type", "INFO"},
				      {"message", "succes"},
				      {"operation", "INVITE"},
				      {"roomname", id_sala} });
    string mensaje = remitente.get_id();
    mensaje += " te invita al cuarto '";
    mensaje.append(id_sala) += "'";
    invitacion = crea_respuesta_json({ {"type", "INVITATION"},
				       {"message", mensaje},
				       {"username", remitente.get_id()},
				       {"roomname", id_sala} });
    salas[id_sala].agrega_invitado(clientes[id_invitado].get_usuario());
    send(remitente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
    send(clientes[id_invitado].get_conexion(), invitacion.c_str(), invitacion.size() + 1, 0);
    return true;
  }
}

/*
 * Envía un mensaje al cliente indicando que el usuario no existe.
 */
void servidor::Servidor::envia_error_usuario(Cliente &cliente, string no_existente, string operacion) {
  string respuesta;
  string mensaje = "El usuario '";
  mensaje += no_existente;
  mensaje += "' no esxiste";
  respuesta = crea_respuesta_json({ {"type", "WARNING"},
				    {"message", mensaje},
				    {"operation", operacion},
				    {"username", no_existente} });
  send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
}

/*
 * Agrega a un usuario a la sala.
 */
void servidor::Servidor::agrega_a_sala(Cliente &cliente, sala::Sala &sala) {
  string respuesta;
  if (!sala.hay_invitacion(cliente.get_usuario()) and sala.get_creador() != cliente) {
    string mensaje = "El usuario no ha sido invitado al cuarto '";
    mensaje.append(sala.get_id()) += "'";
    respuesta = crea_respuesta_json({ {"type", "WARNING"},
				      {"message", mensaje},
				      {"operation", "JOIN_ROOM"},
				      {"roomname", sala.get_id()} });
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  } else if (sala.es_miembro(cliente)) {
    string mensaje = "El usuario ya se unió al cuarto '";
    mensaje.append(sala.get_id()) += "'";
    respuesta = crea_respuesta_json({ {"type", "WARNING"},
				      {"message", mensaje},
				      {"operation", "JOIN_ROOM"},
				      {"roomname", sala.get_id()} });
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  } else {
    respuesta = crea_respuesta_json({ {"type", "INFO"},
				      {"message", "succes"},
				      {"operation", "JOIN_ROOM"},
				      {"roomname", sala.get_id()} });
    string aviso = crea_respuesta_json({ {"type", "JOINED_ROOM"},
					 {"roomname", sala.get_id()},
					 {"username", cliente.get_id()} }); 
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
    for (auto& [key, val] : sala.get_miembros()) {
      if (val != cliente) 
	send(val.get_conexion(), aviso.c_str(), aviso.size() + 1 , 0);            
    }
    sala.agrega_miembro(cliente);
  }
}

/*
 * Envía los miembros de una sala a un cliente.
 */
void servidor::Servidor::envia_miembros(Cliente &cliente, std::string id_sala) {
  string respuesta;
  if (!salas[id_sala].es_miembro(cliente)) {
    string mensaje = "El usuario no se ha unido al cuarto '";
    mensaje.append(id_sala) += "'";
    respuesta = crea_respuesta_json({ {"type", "WARNING"},
				      {"message", mensaje},
				      {"operation", "ROOM_USERS"},
				      {"roomname", id_sala} });
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  } else {
    string miembros = "[ ";
    for (auto& [key, val] : salas[id_sala].get_miembros()) {
      miembros += "\"";
      miembros.append(key);
      miembros += "\"";
      miembros += ",";	
    }
    miembros.pop_back();
    miembros += " ]";
    respuesta = crea_respuesta_json({ {"type", "ROOM_USER_LIST"},
				    {"usernames", miembros} });
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  }
}

/*
 * Envía un mensaje a los usuarios de una sala.
 */
void servidor::Servidor::envia_mensaje_sala(Cliente &cliente, sala::Sala &sala, std::string mensaje) {
  string respuesta, id_sala = sala.get_id();
  if (!salas[id_sala].hay_invitacion(cliente.get_usuario()) and !salas[id_sala].es_miembro(cliente)) {
    string mensaje = "El usuario no se ha unido al cuarto '";
    mensaje.append(sala.get_id()) += "'";
    respuesta = crea_respuesta_json({ {"type", "WARNING"},
				      {"message", mensaje},
				      {"operation", "ROOM_MESSAGE"},
				      {"roomname", sala.get_id()} });
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  } else {
    string respuesta = crea_respuesta_json({ {"type", "ROOM_MESSAGE_FROM"},
					     {"roomname", id_sala},
					     {"username", cliente.get_id()},
					     {"message", mensaje} });
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
  string respuesta, id_sala = sala.get_id();
  if (!sala.es_miembro(cliente)) {
    string mensaje = "El usuario no se ha unido al cuarto '";
    mensaje.append(sala.get_id()) += "'";
    respuesta = crea_respuesta_json({ {"type", "WARNING"},
				      {"message", mensaje},
				      {"operation", "LEAVE_ROOM"},
				      {"roomname", sala.get_id()} });
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
  } else {
    respuesta = crea_respuesta_json({ {"type", "INFO"},
				      {"message", "succes"},
				      {"operation", "LEAVE_ROOM"},
				      {"roomname", id_sala} });
    string aviso = crea_respuesta_json({ {"type", "LEFT_ROOM"},
					 {"roomname", sala.get_id()},
					 {"username", cliente.get_id()} }); 
    send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
    sala.elimina_miembro(cliente);
    if (sala.es_vacia()) {
      salas.erase(sala.get_id());
    } else {
      for (auto& [key, val] : sala.get_miembros()) {
	send(val.get_conexion(), aviso.c_str(), aviso.size() + 1 , 0);
      }
    }  
  }
}

/*
 * Notifica a los usuarios que alguien se desconectó y saca a ese
 * cliente de las salas en donde estaba.
 */
void servidor::Servidor::notifica_salida(Cliente &cliente) {
  for (auto& [key, val] : salas) {
    if (val.es_miembro(cliente))
      saca_de_sala(cliente, val);
    if (salas.empty()) 
      break;
  }
  string aviso = crea_respuesta_json({ {"type", "DISCONNECTED"},
				       {"username", cliente.get_id()} });
  for (auto& [key, val] : clientes) {
    send(val.get_conexion(), aviso.c_str(), aviso.size() + 1 , 0);
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

/*
 * Recibe mensajes y decide a qué método llamar dependiendo
 * del mensaje recibido.
 */
bool servidor::Servidor::recibe_mensaje(cliente::Cliente cliente) {
  char buffer[4096];

  while (true) {
    memset(buffer, 0, 4096);
    int msj_recibido = recv(cliente.get_conexion(), buffer, 4096, 0);
    
    if (msj_recibido == -1) {
      throw runtime_error("Ocurrió un error al recibir el mensaje.");
    }
    
    if (msj_recibido == 0){
      cout << "Cliente " << cliente.get_id() << " desconectado" << endl;
      cierra_cliente(cliente.get_id());
      return false;
    }
    
    string mensaje = string(buffer, 0, msj_recibido);
    
    cout << mensaje << endl;
    
    Json::Reader reader;
    Json::Value msj_json;
    
    reader.parse(mensaje, msj_json);
    
    if (!msj_json.isMember("type")) {
      cierra_cliente(cliente.get_id());
      return false;
    }
    
    switch (mensaje::tabla[msj_json["type"].asString()]) {
      
    case mensaje::Mensaje::IDENTIFY:
      if (!msj_json.isMember("username")) {
	cierra_cliente(cliente.get_id());
	return false;
      }
      identifica_cliente(cliente, msj_json["username"].asString());
      break;
      
    case mensaje::Mensaje::STATUS:
      if (!msj_json.isMember("status")) {
	cierra_cliente(cliente.get_id());
	return false;
      }
      switch (estado::tabla[msj_json["status"].asString()]) {	
      case estado::Estado::AWAY:
	asigna_estado(cliente, estado::Estado::AWAY);
	break;
	
      case estado::Estado::ACTIVE:
	asigna_estado(cliente, estado::Estado::ACTIVE);
	break;
	
      case estado::Estado::BUSY:
	asigna_estado(cliente, estado::Estado::BUSY);
	break;
	
      default:
	cierra_cliente(cliente.get_id());
	return false;
      }
      break;    
      
    case mensaje::Mensaje::USERS:
      envia_usuarios(cliente);
      break;
      
    case mensaje::Mensaje::MESSAGE:
      if (!msj_json.isMember("username") || !msj_json.isMember("message")) {
	cierra_cliente(cliente.get_id());
	return false;
      }
      if (clientes.find(msj_json["username"].asString()) == clientes.end()) {
	envia_error_usuario(cliente, msj_json["username"].asString(),
			    mensaje::a_string(mensaje::Mensaje::MESSAGE));
      } else {
	envia_mensaje_privado(cliente, msj_json["username"].asString(), msj_json["message"].asString());
      }
      break;
      
    case mensaje::Mensaje::PUBLIC_MESSAGE:
      if (!msj_json.isMember("message")) {
	cierra_cliente(cliente.get_id());
	return false;
      }
      envia_mensaje_publico(cliente, msj_json["message"].asString());
      break;
      
    case mensaje::Mensaje::NEW_ROOM: {
      if (!msj_json.isMember("roomname")) {
	cierra_cliente(cliente.get_id());
	return false;
      }
      sala::Sala sala(cliente, msj_json["roomname"].asString());
      agrega_sala(cliente, sala);
      break;
    }
      
    case mensaje::Mensaje::INVITE: {
      if (!msj_json.isMember("roomname") || !msj_json.isMember("usernames")) {
	cierra_cliente(cliente.get_id());
	return false;
      }
      
      Json::Value invitados = msj_json["usernames"];
      if (invitados.empty()) {
	cierra_cliente(cliente.get_id());
	return false;
      }
      
      bool existen_usuarios = true;
      for (uint i = 0; i < invitados.size(); i++) {
	if (clientes.find(invitados[i].asString()) == clientes.end()) {
	  envia_error_usuario(cliente, invitados[i].asString(), mensaje::a_string(mensaje::Mensaje::INVITE));
	  existen_usuarios = false;
	  break;
	}
      }
      
      if (!existe_sala(msj_json["roomname"].asString())) {
      envia_error_sala(cliente, msj_json["roomname"].asString(), mensaje::a_string(mensaje::Mensaje::INVITE));
      break;
      }
      
      if (existen_usuarios) {
	for (uint i = 0; i < invitados.size(); i++) 
	  invita_a_sala(cliente, invitados[i].asString(), msj_json["roomname"].asString());
    }
      break;
    }
      
    case mensaje::Mensaje::JOIN_ROOM:
      if (!msj_json.isMember("roomname")) {
	cierra_cliente(cliente.get_id());
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
  string mensaje = "El cuarto '";
  mensaje.append(no_existente) += "' no existe";
  respuesta = crea_respuesta_json({ {"type", "WARNING"},
				    {"message", mensaje},
				    {"operation", operacion},
				    {"roomname", no_existente} });
  send(cliente.get_conexion(), respuesta.c_str(), respuesta.size() + 1 , 0);
}
