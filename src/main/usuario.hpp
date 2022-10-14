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
    Usuario(std::string nombre);

    /*
     * Nos dice si son iguales.
     */
    bool operator==(const Usuario& otro) const;
    
    /*
     * Nos dice si son diferentes.
     */
    bool operator!=(const Usuario& otro) const;

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
