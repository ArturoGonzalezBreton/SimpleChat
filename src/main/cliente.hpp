#ifndef CLIENTE_H
#define CLIENTE_H

#include "estado.hpp"
#include "mensaje.hpp"
#include "usuario.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

namespace cliente {

  class Cliente {

  private:
    // Conexión.
    int conexion;
    // Puerto al que se conectará el ciente.
    int puerto;
    // Socket para conectarse con el servidor.
    sockaddr_in enchufe;
    // Dirección a la que se conectará el cliente.
    std::string direccion_ip;
    // Usuario del cliente.
    usuario::Usuario usuario;
    // Estado del cliente.
    estado::Estado estado;

  public:
    Cliente() {}

    /*
     * Nos dice si son iguales.
     */
    bool operator==(const Cliente& rhs) const;

    /*
     * Nos dice si son diferentes.
     */
    bool operator!=(const Cliente& rhs) const;
    
    /*
     * Constructor que recibe únicamente el puerto.
     */
    Cliente(int puerto);

    /*
     * Constructor que recibe el puerto y la direccion ip.
     */
    Cliente(int puerto, std::string direccion_ip);
      
    /*
     * Constructor que recibe el puerto al que se conectará, la dirección ip
     * del servidor y el identificador del usuario del cliente.
     */   
    Cliente(int puerto, std::string direccion_ip, std::string id);

    /*
     * Crea un socket y lo une al puerto en el que el servidor
     * escuchará conexiones.
     */
    int crea_conexion();

    /*
     * Se conecta al servidor.
     */
    void conecta();

    /*
     * Envía mensajes al servidor.
     */
    void envia_mensaje(std::string mensaje);

    /*
     * Recibe mensajes del servidor.
     */
    std::string recibe_mensajes();
    
    /*
     * Desconecta al cliente.
     */
    void desconecta();

    /*
     * Devuelve la conexión del lado del cliente.
     */
    int get_conexion();

    /*
     * Asigna la conexión del lado del cliente.
     */
    void set_conexion(int conexion);

    /*
     * Devuelve el socket del cliente.
     */
    sockaddr_in get_socket();

    /*
     * Asigna un enchufe al cliente.
     */
    void set_socket(sockaddr_in enchufe);
  
    /*
     * Devuelve el identificador del usuario.
     */
    std::string get_id();

    /*
     * Devuelve el identificador del usuario.
     */
    void set_id(std::string id);

    /*
     * Devuelve el usuario.
     */
    usuario::Usuario get_usuario();

    /*
     * Devuelve el estado del cliente.
     */
    estado::Estado get_estado();

    /*
     * Asigna estado al cliente.
     */
    void set_estado(estado::Estado estado);

    /*
     * Asigna ip del servidor a conectarse.
     */
    void set_ip(std::string ip);

    /*
     * Devuelve ip del servidor a conectarse.
     */
    std::string get_ip();

    /*
     * Asigna puerto del servidor a conectarse.
     */
    void set_puerto(int puerto);

    /*
     * Devuelve puerto del servidor a conectarse.
     */
    int get_puerto();
  };
}

#endif
