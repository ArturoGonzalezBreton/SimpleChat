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

  // Convierte un estado a su representación en cadena.
  std::string a_string(Estado estado);
}

#endif
