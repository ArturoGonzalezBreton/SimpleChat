#include "usuario.hpp"
#include <string>

using namespace std;
using namespace usuario;

/*
 * Devuelve el identificador del usuario.
 */
string Usuario::get_nombre() {
  return this -> nombre;
}

/*
 * Asigna identificador al usuario.
 */
void Usuario::set_nombre(std::string nombre) {
  this -> nombre = nombre;
}

/*
 * Nos dice si son iguales.
 */
bool Usuario::operator==(const Usuario& otro) const {
  return nombre == otro.nombre;
}

/*
 * Nos dice si son diferentes.
 */
bool Usuario::operator!=(const Usuario& otro) const {
  return nombre != otro.nombre;
}
