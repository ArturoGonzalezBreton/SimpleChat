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
#include <map>

namespace servidor {

  using namespace std;
  using namespace cliente;
  
  class Servidor {
    
  private:    
    // Conexión
    int conexion;
    // Socket.
    sockaddr_in enchufe;
    // Puerto en el que se reciben conexiones.
    int puerto;
    // Clientes conectados.
    map<string, Cliente> clientes;
    // Salas.
    list<sala::Sala> salas;

    Servidor() {}
    
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
     * Asigna identificador al cliente.
     */
    void identifica_cliente(Cliente cliente, string id);
    
    /*
     * Cierra la conexion.
     */
    void cierra_socket(int num_conexion);

    /*
     * Recibe mensajes y decide a qué método llamar dependiendo del mensaje recibido.
     */
    void sirve(Cliente cliente);

    /*
     * Cierra la conexion del cliente;
     */
    void cierra_cliente(string id);
  };
}
