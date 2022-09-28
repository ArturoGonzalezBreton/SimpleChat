#include "cliente.hpp"
#include "sala.hpp"
#include <string>

using namespace std;
using namespace sala;

/*
 * Devuelve el creador de la sala.
 */
cliente::Cliente Sala::get_creador() {
  return this -> creador;
}  
 
/*
 * Devuelve el nombre de la sala.
 */
string Sala::get_id() {
  return this -> nombre;
}

/*
 * Devuelve la los miembros de la sala.
 */
map<string, cliente::Cliente> Sala::get_miembros() {
  return this -> miembros;
}

/*
 * Regresa la lista de invitaciones.
 */
std::list<Sala::Invitacion> Sala::get_invitaciones() {
  return this -> invitaciones;
}

/*
 * Busca una invitación.
 */
bool Sala::hay_invitacion(usuario::Usuario invitado, usuario::Usuario invitador) {
  Invitacion invitacion(invitado.get_nombre(), invitador.get_nombre());

  list<Invitacion>::iterator it;
  for (it = invitaciones.begin(); it != invitaciones.end(); it++) {
    if (*it == invitacion) return true;
  }

  return false;
}

/*
 * Regresa la lista de invitados.
 */
std::list<usuario::Usuario> Sala::get_invitados() {
  return this -> invitados;
}

/*
 * Busca un invitado.
 */
bool Sala::hay_invitacion(usuario::Usuario invitado) {
  list<usuario::Usuario>::iterator it;
  
  for (it = invitados.begin(); it != invitados.end(); it++) {
    if (*it == invitado) return true;
  }

  return false;
}

/*
 * Agrega un invitado.
 */
void Sala::agrega_invitado(usuario::Usuario invitado) {
  if (!hay_invitacion(invitado))
    this -> invitados.push_back(invitado);     
}

/*
 * Busca un miembro en la sala.
 */
bool Sala::es_miembro(cliente::Cliente &miembro) {
  return this -> miembros.find(miembro.get_id()) != miembros.end();
}

/*
 * Elimina a un miembro.
 */
void Sala::elimina_miembro(cliente::Cliente &cliente) {
  if (this -> miembros.find(cliente.get_id()) != miembros.end()) 
      this -> miembros.erase(cliente.get_id());
}

/*
 * Agrega un miembro.
 */
bool Sala::agrega_miembro(cliente::Cliente &cliente) {
  if (this -> miembros.find(cliente.get_id()) == miembros.end()) {
    this -> miembros.insert({cliente.get_id(), cliente});
    return true;
  }
  return false;
} 

/*
 * Verifica si hay usuarios en la sala.
 */
bool Sala::es_vacia() {
  return this -> miembros.empty();
}

/*
 * Elimina a un invitado de la lista de invitados,
 */
void Sala::elimina_invitacion(usuario::Usuario usuario) {
  //invitados.erase(usuario);
}
