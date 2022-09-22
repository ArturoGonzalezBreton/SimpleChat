#include "cliente.hpp"
#include "sala.hpp"
#include <string>

using namespace std;
using namespace sala;

/*
 * Devuelve el nombre de la sala.
 */
string Sala::get_nombre() {
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
 * Busca un miembro en la sala.
 */
bool Sala::es_miembro(cliente::Cliente miembro) {
  return miembros.find(miembro.get_id()) != miembros.end();
}

/*
 * Elimina a un miembro.
 */
void Sala::elimina_miembro(cliente::Cliente cliente) {
  if (miembros.find(cliente.get_id()) != miembros.end()) 
      miembros.erase(cliente.get_id());
}
