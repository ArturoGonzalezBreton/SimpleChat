#ifndef ESTADO_H
#define ESTADO_H

#include <string>
#include <map>

namespace estado {

  enum Estado {
    ACTIVE,
    AWAY,
    BUSY
  };
  
  static std::map<std::string, Estado> tabla = {
    {"ACTIVE", ACTIVE}, {"AWAY", AWAY}, {"BUSY", BUSY}
  };
  
  std::string a_string(Estado estado);
}

#endif
