#ifndef USUARIO_H
#define USUARIO_H

#include <string>

namespace usuario {
  
  class Usuario {
    
  private:
    // Identificador del usuario.
    std::string nombre;
    
  public:

    Usuario() {}
    
    /*
     * Constructor que recibe nombre.
     */
    Usuario(std::string nombre) {
      this -> nombre = nombre;
    }

    /*
     * Devuelve el identificador del usuario.
     */    
    std::string get_nombre();

    /*
     * Asigna identificador al usuario.
     */
    void set_nombre(std::string nombre);
  };
}
  
#endif
