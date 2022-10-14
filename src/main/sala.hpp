#ifndef SALA_H
#define SALA_H

#include "cliente.hpp"
#include "usuario.hpp"
#include <string>
#include <map>
#include <list>

namespace sala {
  
  class Sala {

  private:
      
    // Nombre dde la sala.
    std::string nombre;
    // Creador de la sala.
    cliente::Cliente creador;
    // Usuarios en la sala.{
    std::map<std::string, cliente::Cliente> miembros;
    // Lista de invitados.
    std::list<usuario::Usuario> invitados;
    
  public:

    Sala() {}

    Sala(cliente::Cliente &creador, std::string nombre);

    /*
     * Devuelve el creador de la sala.
     */
    cliente::Cliente get_creador();
    
    /*
     * Devuelve el nombre de la sala.
     */
    std::string get_id();

    /*
     * Devuelve la los miembros de la sala.
     */
    std::map<std::string, cliente::Cliente> get_miembros();

    /*
     * Busca un miembro en la sala.
     */
    bool es_miembro(cliente::Cliente &miembro);

    /*
     * Regresa la lista de invitados.
     */
    std::list<usuario::Usuario> get_invitados();

    /*
     * Busca un invitado.
     */
    bool hay_invitacion(usuario::Usuario invitado);

    /*
     * Agrega un invitado.
     */
    void agrega_invitado(usuario::Usuario invitado);
    
    /*
     * Elimina a un miembro.
     */
    void elimina_miembro(cliente::Cliente &cliente);

    /*
     * Agrega un miembro.
     */
    void agrega_miembro(cliente::Cliente &cliente);

    /*
     * Verifica si hay usuarios en la sala.
     */
    bool es_vacia();
  };
}

#endif
