#ifndef MENSAJE_H
#define MENSAJE_H

#include <string>
#include <map>

namespace mensaje {

  enum Mensaje {
    ERROR,
    WARNING,
    INFO,
    IDENTIFY,
    NEW_USER,
    STATUS,
    NEW_STATUS,
    USERS,
    USER_LIST,
    MESSAGE,
    MESSAGE_FROM,
    PUBLIC_MESSAGE,
    PUBLIC_MESSAGE_FROM,
    NEW_ROOM,
    INVITE,
    INVITATION,
    JOIN_ROOM,
    JOINED_ROOM,
    ROOM_USERS,
    ROOM_USER_LIST,
    ROOM_MESSAGE,
    ROOM_MESSAGE_FROM,
    LEAVE_ROOM,
    LEFT_ROOM,
    DISCONNECT,
    DISCONNECTED,
  };  

  static std::map<std::string, Mensaje>  tabla = {
    {"ERROR", ERROR}, {"WARNING", WARNING},
    {"INFO", INFO}, {"IDENTIFY", IDENTIFY},
    {"NEW_USER", NEW_USER}, {"STATUS", STATUS},
    {"NEW_STATUS", NEW_STATUS}, {"USERS", USERS},
    {"USER_LIST", USER_LIST}, {"MESSAGE", MESSAGE},
    {"MESSAGE_FROM", MESSAGE_FROM}, {"PUBLIC_MESSAGE", PUBLIC_MESSAGE},
    {"PUBLIC_MESSAGE_FROM", PUBLIC_MESSAGE_FROM}, {"NEW_ROOM", NEW_ROOM},
    {"INVITE", INVITE}, {"INVITATION", INVITATION},
    {"JOIN_ROOM", JOIN_ROOM}, {"JOINED_ROOM", JOINED_ROOM},
    {"ROOM_USERS", ROOM_USERS}, {"ROOM_USER_LIST", ROOM_USER_LIST},
    {"ROOM_MESSAGE", ROOM_MESSAGE}, {"ROOM_MESSAGE_FROM", ROOM_MESSAGE_FROM},
    {"LEAVE_ROOM", LEAVE_ROOM}, {"LEFT_ROOM", LEFT_ROOM},
    {"DISCONNECT", DISCONNECT}, {"DISCONNECTED", DISCONNECTED},
    {"NEW_STATUS", NEW_STATUS}
  };

  // Devuelve la representación en cadena de un mensaje.
  std::string a_string(Mensaje mensaje);
}

#endif
