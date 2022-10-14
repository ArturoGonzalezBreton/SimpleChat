#include "sala.hpp"

using namespace std;

sala::Sala::Sala(cliente::Cliente &creador, std::string nombre) {
  this -> creador = creador;
  this -> nombre = nombre;
  this -> miembros.insert({creador.get_id(), creador});
}

/*
 * Devuelve el creador de la sala.
 */
cliente::Cliente sala::Sala::get_creador() {
  return this -> creador;
}  
 
/*
 * Devuelve el nombre de la sala.
 */
string sala::Sala::get_id() {
  return this -> nombre;
}

/*
 * Devuelve la los miembros de la sala.
 */
map<string, cliente::Cliente> sala::Sala::get_miembros() {
  return this -> miembros;
}

/*
 * Regresa la lista de invitados.
 */
std::list<usuario::Usuario> sala::Sala::get_invitados() {
  return this -> invitados;
}

/*
 * Busca un invitado.
 */
bool sala::Sala::hay_invitacion(usuario::Usuario invitado) {
  list<usuario::Usuario>::iterator it;
  
  for (it = invitados.begin(); it != invitados.end(); it++) {
    if (*it == invitado) return true;
  }

  return false;
}

/*
 * Agrega un invitado.
 */
void sala::Sala::agrega_invitado(usuario::Usuario invitado) {
  if (!hay_invitacion(invitado))
    this -> invitados.push_back(invitado);     
}

/*
 * Busca un miembro en la sala.
 */
bool sala::Sala::es_miembro(cliente::Cliente &miembro) {
  return this -> miembros.find(miembro.get_id()) != miembros.end();
}

/*
 * Elimina a un miembro.
 */
void sala::Sala::elimina_miembro(cliente::Cliente &cliente) {
  this -> miembros.erase(cliente.get_id());
  this -> invitados.remove(cliente.get_usuario());
}

/*
 * Agrega un miembro.
 */
void sala::Sala::agrega_miembro(cliente::Cliente &cliente) {
  this -> miembros.insert({cliente.get_id(), cliente});;
} 

/*
 * Verifica si hay usuarios en la sala.
 */
bool sala::Sala::es_vacia() {
  return this -> miembros.empty();
}
