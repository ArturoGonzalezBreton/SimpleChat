#include "mensaje.hpp"
#include <string>
#include <map>

using namespace mensaje;
using namespace std;

string get_key(Mensaje mensaje);

string mensaje::a_string(Mensaje mensaje) {
  return get_key(mensaje);
}

string get_key(Mensaje mensaje) {
  map<string, Mensaje>::iterator it;
  
  for (it = tabla.begin(); it != tabla.end(); it++) {
    if (it -> second == mensaje) return it -> first;
  }
  
  return NULL;
} 
