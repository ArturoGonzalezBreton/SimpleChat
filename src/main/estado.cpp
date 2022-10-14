#include "estado.hpp"

using namespace estado;
using namespace std;

string get_key(Estado estado);

// Convierte un estado a su representación en cadena.
string estado::a_string(Estado estado) {
  return get_key(estado);
}

string get_key(Estado estado) {
  map<string, Estado>::iterator it;
  
  for (it = tabla.begin(); it != tabla.end(); it++) {
    if (it -> second == estado) return it -> first;
  }
  
  return NULL;
}
