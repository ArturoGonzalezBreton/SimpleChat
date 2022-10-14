#include "usuario.hpp"

using namespace std;

/*
 * Constructor que recibe nombre.
 */
usuario::Usuario::Usuario(std::string nombre) {
  this -> nombre = nombre;
}

/*
 * Devuelve el identificador del usuario.
 */
string usuario::Usuario::get_nombre() {
  return this -> nombre;
}

/*
 * Asigna identificador al usuario.
 */
void usuario::Usuario::set_nombre(std::string nombre) {
  this -> nombre = nombre;
}

/*
 * Nos dice si son iguales.
 */
bool usuario::Usuario::operator==(const Usuario& otro) const {
  return nombre == otro.nombre;
}

/*
 * Nos dice si son diferentes.
 */
bool usuario::Usuario::operator!=(const Usuario& otro) const {
  return nombre != otro.nombre;
}
