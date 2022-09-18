#ifndef MENSAJE_H
#define MENSAJE_H

#include <string>

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
    NEW_STATUS
  };
  
  static const char *enum_str[] = {"ERROR", "WARNING", "INFO", "IDENTIFY",
				   "NEW_USER", "STATUS", "NEW_STATUS, USERS",
				   "USER_LIST", "MESSAGE", "MESSAGE_FROM", "PUBLIC_MESSAGE",
				   "PUBLIC_MESSAGE_FROM", "NEW_ROOM, INVITE", "INVITATION",
				   "JOIN_ROOM", "JOINED_ROOM", "ROOM_USERS", "ROOM_USER_LIST",
				   "ROOM_MESSAGE", "ROOM_MESSAGE_FROM", "LEAVE_ROOM, LEFT_ROOM",
				   "DISCONNECT", "DISCONNECTED", "NEW_STATUS"};
  
  std::string to_string(int estado);
  
}

#endif
