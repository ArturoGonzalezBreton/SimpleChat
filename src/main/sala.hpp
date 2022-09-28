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

    // Clase interna privada para invitaciones.
    class Invitacion {

    private:
      
      usuario::Usuario invitado;
      usuario::Usuario invitador;
      
    public:
      
      Invitacion() {}
      
      Invitacion(std::string id_invitado, std::string id_invitador) {
	this -> invitado = usuario::Usuario(id_invitado);
	this -> invitador = usuario::Usuario(id_invitador);
      }

      /*
       * Nos dice si son iguales.
       */
      
      usuario::Usuario get_invitado() const {
	return this -> invitado;
      }

      usuario::Usuario get_invitador() const {
	return this -> invitador;
      }
      
      bool operator==(const Invitacion& otra) const {
	return (this -> invitado) == (otra.get_invitado()) and (this -> invitador) == (otra.get_invitador());
      }      
    };
      
    // Nombre dde la sala.
    std::string nombre;
    // Creador de la sala.
    cliente::Cliente creador;
    // Usuarios en la sala.
    std::map<std::string, cliente::Cliente> miembros;
    // Lista de invitaciones.
    std::list<Invitacion> invitaciones;
    // Lista de invitados.
    std::list<usuario::Usuario> invitados;
    
  public:

    Sala() {}

    Sala(cliente::Cliente creador, std::string nombre) {
      this -> creador = creador;
      this -> nombre = nombre;
      this -> miembros.insert({creador.get_id(), creador});
    }

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
     * Busca una invitación.
     */
    bool hay_invitacion(usuario::Usuario invitado, usuario::Usuario invitador);

    /*
     * Regresa la lista de invitaciones.
     */
    std::list<Invitacion> get_invitaciones();

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
    bool agrega_miembro(cliente::Cliente &cliente);

    /*
     * Verifica si hay usuarios en la sala.
     */
    bool es_vacia();

    /*
     * Elimina a un invitado de la lista de invitados,
     */
    void elimina_invitacion(usuario::Usuario usuario);
  };
}

#endif
