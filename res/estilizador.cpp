#include "estilizador.hpp"
#include "../src/main/mensaje.hpp"

using namespace std;
using namespace estilizador;

/*
 * Los métodos a continuación, cambian el color de la cadena
 * y dependiendo del valor del entero que reciben le dan formato.
 */

std::string estilizador::rojo(std::string cadena, int formato) {
  return cadena.insert(0, ";31m").insert(0, to_string(formato)).insert(0, "\033[").append("\033[0m");
}

std::string estilizador::amarillo(std::string cadena, int formato) {
  return cadena.insert(0, ";33m").insert(0, to_string(formato)).insert(0, "\033[").append("\033[0m");
}

std::string estilizador::verde(std::string cadena, int formato) {
  return cadena.insert(0, ";32m").insert(0, to_string(formato)).insert(0, "\033[").append("\033[0m");
}

std::string estilizador::azul(std::string cadena, int formato) {
  return cadena.insert(0, ";34m").insert(0, to_string(formato)).insert(0, "\033[").append("\033[0m");
}

std::string estilizador::magenta(std::string cadena, int formato) {
  return cadena.insert(0, ";35m").insert(0, to_string(formato)).insert(0, "\033[").append("\033[0m");
}

std::string estilizador::turquesa(std::string cadena, int formato) {
  return cadena.insert(0, ";36m").insert(0, to_string(formato)).insert(0, "\033[").append("\033[0m");
}

/*
 * Da estilo a la cadena dependiendo del tipo
 * de mensaje.
 */
string estilizador::estiliza(string mensaje) {
   Json::Reader reader;
   Json::Value archivo;
   
  reader.parse(mensaje, archivo);

  if (!archivo.isMember("type")) return mensaje.insert(0, "Se recibió un mensaje inválido:\n");

  string type = archivo["type"].asString();
  switch (mensaje::tabla[type]) {
    
  case mensaje::Mensaje::ERROR:
    if (!archivo.isMember("message"))
      return mensaje.insert(0, "Se recibió un mensaje inválido:\n");
    return rojo(type, 1).append(": ").append(archivo["message"].asString());
    break;
  case mensaje::Mensaje::WARNING:
    if (!archivo.isMember("message"))
      return mensaje.insert(0, "Se recibió un mensaje inválido:\n");
    return amarillo(type, 1).append(": ").append(archivo["message"].asString());
    break;

  case mensaje::Mensaje::INFO: {
    if (!archivo.isMember("message") || !archivo.isMember("operation"))
      return mensaje.insert(0, "Se recibió un mensaje inválido:\n");
    string operation = archivo["operation"].asString();
    return verde(type, 1).append(": ").append("succeeded ").append(operation).append(" operation");
    break;
  }
    
  case mensaje::Mensaje::NEW_USER:
    if (!archivo.isMember("username"))
      return mensaje.insert(0, "Se recibió un mensaje inválido:\n");
    return azul(type, 0).append(": ").append(archivo["username"].asString());
    break;

  case mensaje::Mensaje::NEW_STATUS: {
    if (!archivo.isMember("username") || !archivo.isMember("status"))
      return mensaje.insert(0, "Se recibió un mensaje inválido:\n");
    string username = archivo["username"].asString();
    string status = archivo["status"].asString();
    return azul(type, 0).append(": ").append(username).append(" is ").append(status);
    break;
  }
    
  case mensaje::Mensaje::USER_LIST: {
    if (!archivo.isMember("usernames"))
      return mensaje.insert(0, "Se recibió un mensaje inválido:\n");
    Json::Value users = archivo["usernames"];
    string usuarios = "";
    for (uint i = 0; i < users.size(); i++) {
      usuarios += users[i].asString();
      usuarios += ", ";
    }
    usuarios.pop_back();
    usuarios.pop_back();
    return azul(type, 0).append(": ").append(usuarios);
  }

  case mensaje::Mensaje::MESSAGE_FROM: {
    if (!archivo.isMember("message") || !archivo.isMember("username"))
      return mensaje.insert(0, "Se recibió un mensaje inválido:\n");
    string username = archivo["username"].asString();
    string message = archivo["message"].asString();
    return azul(type, 0).append(" ").append(username).append(": ").append(message);
  }
    
  case mensaje::Mensaje::PUBLIC_MESSAGE_FROM: {
    if (!archivo.isMember("message") || !archivo.isMember("username"))
      return mensaje.insert(0, "Se recibió un mensaje inválido:\n");
    string username = archivo["username"].asString();
    string message = archivo["message"].asString();
    return turquesa(type, 0).append(" ").append(username).append(": ").append(message);
  }
    
  case mensaje::Mensaje::JOINED_ROOM: {
    if (!archivo.isMember("roomname") || !archivo.isMember("username"))
      return mensaje.insert(0, "Se recibió un mensaje inválido:\n");
    string username = archivo["username"].asString();
    string roomname = archivo["roomname"].asString();
    return magenta(type, 0).append(": ").append(username).append(" joined ").append(roomname);
  }
    
  case mensaje::Mensaje::ROOM_USER_LIST: {
    if (!archivo.isMember("usernames"))
      return mensaje.insert(0, "Se recibió un mensaje inválido:\n");
    Json::Value users = archivo["usernames"];
    string usuarios = "";
    for (uint i = 0; i < users.size(); i++) {
      usuarios += users[i].asString();
      usuarios += ", ";
    }
    usuarios.pop_back();
    usuarios.pop_back();
    return magenta(type, 0).append(": ").append(usuarios);
  }
    
  case mensaje::Mensaje::ROOM_MESSAGE_FROM: {
    if (!archivo.isMember("message") || !archivo.isMember("username") || !archivo.isMember("roomname"))
      return mensaje.insert(0, "Se recibió un mensaje inválido:\n");
    string username = archivo["username"].asString();
    string message = archivo["message"].asString();
    string roomname = archivo["roomname"].asString();
    string cadena = magenta(type, 0).append(" ").append(username).append(" in ").append(roomname);
    cadena.append(": ").append(message);
    return cadena;
  }

  case mensaje::Mensaje::INVITATION: {
    if (!archivo.isMember("message") || !archivo.isMember("username") || !archivo.isMember("roomname"))
      return mensaje.insert(0, "Se recibió un mensaje inválido:\n");
    string username = archivo["username"].asString();
    string message = archivo["message"].asString();
    string roomname = archivo["roomname"].asString();
    return magenta(type, 0).append(": ").append(message);
  }
    
  case mensaje::Mensaje::LEFT_ROOM: {
    if (!archivo.isMember("roomname") || !archivo.isMember("username"))
      return mensaje.insert(0, "Se recibió un mensaje inválido:\n");
    string username = archivo["username"].asString();
    string roomname = archivo["roomname"].asString();
    return magenta(type, 0).append(": ").append(username).append(" left ").append(roomname);
  }
    
  case mensaje::Mensaje::DISCONNECTED:
    if (!archivo.isMember("username"))
      return mensaje.insert(0, "Se recibió un mensaje inválido:\n");
    return azul(type, 0).append(": ").append(archivo["username"].asString());
    break;    
  
  default:
    return mensaje.insert(0, "Se recibió un mensaje inválido:\n");;
  }
}
