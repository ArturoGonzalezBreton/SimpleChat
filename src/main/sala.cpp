#include "sala.hpp"
#include <string>

using namespace std;
using namespace sala;

/*
 * Devuelve el nombre de la sala.
 */
std::string Sala::get_nombre() {
  return this -> nombre;
}
