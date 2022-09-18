#ifndef ESTADO_H
#define ESTADO_H

#include <string>

namespace estado {

  enum Estado {
    ACTIVE,
    AWAY,
    BUSY
  };
  
  static const char *enum_str[] = {"ACTIVE", "AWAY", "BUSY"};
  
  std::string to_string(int estado);
  
}

#endif
