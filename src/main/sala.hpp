#ifndef SALA_H
#define SALA_H

#include <string>

namespace sala {
  
  class Sala {

  private:
    // Nombre dde la sala.
    std::string nombre;

  public:

    Sala() {}

    /*
     * Devuelve el nombre de la sala.
     */
    std::string get_nombre();
  };
}

#endif
