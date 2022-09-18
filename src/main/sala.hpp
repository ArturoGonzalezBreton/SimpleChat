#ifndef SALA_H
#define SALA_H

#include "cliente.hpp"
#include <string>
#include <map>

namespace sala {
  
  class Sala {

  private:
    // Nombre dde la sala.
    std::string nombre;
    // Creador de la sala.
    cliente::Cliente creador;
    // Usuarios en la sala.
    std::map<std::string, cliente::Cliente> miembros;
    
  public:

    Sala() {}

    Sala(cliente::Cliente creador, std::string nombre) {
      this -> creador = creador;
      this -> nombre = nombre;
      this -> miembros.insert({creador.get_id(), creador});
    }

    /*
     * Devuelve el nombre de la sala.
     */
    std::string get_nombre();
  };
}

#endif
