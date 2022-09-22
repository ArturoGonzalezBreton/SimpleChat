#ifndef SERVER_H
#define SERVER_H

#include "estado.hpp"
#include "mensaje.hpp"
#include "sala.hpp"
#include "cliente.hpp"
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <list>
#include <thread>
#include <map>

namespace servidor {
  
  class Servidor {
    
  private:    
    // Conexión
    int conexion;
    // Socket.
    sockaddr_in enchufe;
    // Puerto en el que se reciben conexiones.
    int puerto;
    // Clientes conectados.
    std::map<std::string, cliente::Cliente> clientes;
    // Salas.
    std::map<std::string, sala::Sala> salas;
    // Hilos.
    std::list<std::thread> hilos;

    Servidor() {}
    
    /*
     * Recibe mensajes del cliente.
     */
    void recibe_mensaje(cliente::Cliente cliente);

    /*
     * Asigna identificador al cliente y lo agrega a la lista.
     */
    void identifica_cliente(cliente::Cliente cliente, std::string id);

    /*
     * Asigna un estado al cliente.
     */
    void asigna_estado(cliente::Cliente cliente, estado::Estado estado);

    /*
     * Envia la lista de usuarios a un cliente.
     */
    void envia_usuarios(cliente::Cliente cliente);

    /*
     * Envía un mensaje privado.
     */
    void envia_mensaje_privado(cliente::Cliente remitente, std::string id_destinatario, std::string mensaje);

    /*
     * Envía un mensaje público.
     */
    void envia_mensaje_publico(cliente::Cliente remitente, std::string mensaje);

    /*
     * Crea una nueva sala.
     */
    void crea_sala(cliente::Cliente creador, std::string id_sala);

    /*
     * Manda invitación a un usuario para unirse a una sala.
     * Regresa false en caso de error.
     */
    bool invita_a_sala(cliente::Cliente remitente, std::string id_invitado, std::string id_sala);

    /*
     * Agrega a un usuario a la sala.
     */
    void agrega_a_sala(cliente::Cliente cliente, std::string id_sala);

    /*
     * Envía los miembros de una sala a un cliente.
     */
    void envia_miembros(cliente::Cliente cliente, std::string id_sala);

    /*
     * Envía un mensaje a los usuarios de una sala.
     */
    void envia_mensaje_sala(cliente::Cliente cliente, std::string id_sala, std::string mensaje);

    /*
     * Saca a un cliente de una sala.
     */
    void saca_de_sala(cliente::Cliente cliente, std::string id_sala);

    /*
     * Notifica a los usuarios que alguien se desconectó.
     */
    void notifica_salida(cliente::Cliente cliente);
    
  public:
    
    /*
     * Constructor que recibe el puerto en el que el servidor
     * Recibirá conexiones.
     */
    Servidor(int puerto) {
      this -> puerto = puerto;   
    }
    
    /*
     * Crea un socket y lo une al puerto en el que escuchará
     * conexiones.
     */
    void crea_conexion();
    
    /*
     * Escucha conexiones de los clientes.
     */
    void escucha();
    
    /*
     * Termina el servidor.
     */
    void cierra_socket();

    /*
     * Recibe mensajes y decide a qué método llamar dependiendo del mensaje recibido.
     */
    void sirve(cliente::Cliente cliente);

    /*
     * Cierra la conexion del cliente;
     */
    void cierra_cliente(std::string id);
  };
}

#endif
