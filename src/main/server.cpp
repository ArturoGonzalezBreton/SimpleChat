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

namespace servidor {

  using namespace std;
  
  class Servidor {
    
  private:    
    // Conexiones. 
    list<int> conexiones;
    // Puerto en el que se reciben conexiones.
    int puerto;
    // Número de conexiones.
    int num_conexiones = 0;
    // Socket del ciente.
    int socket_cliente;
    // Sockets en donde se realizarán las conexiones.
    list<sockaddr_in> sockets;

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
     * Devuelve las conexiones.
     */
    int get_conexion(int num_conexion) {
      return this -> conexiones.front();
    }

    /*
     * Devuelve el número de conexiones
     */
    int get_conexiones() {
      return num_conexiones;
    }
    
    /*
     * Crea un socket y lo une al puerto enb el que escuchará
     * conexiones.
     */
    int crea_conexion() {
      if (this -> num_conexiones == 0) {
	this -> conexiones.push_back(socket(AF_INET, SOCK_STREAM, 0));
	sockaddr_in socket;
	this -> sockets.push_back(socket);
	
	if (conexiones.front() == -1) {
	  throw runtime_error("Ocurrió un error al crear la conexión");
	}

	sockets.front().sin_family = AF_INET;
	sockets.front().sin_port = htons(this -> puerto);
	inet_pton(AF_INET, "0.0.0.0", &(sockets.front().sin_addr));

	bind(conexiones.front(), (sockaddr*)&(sockets.front()), sizeof(socket));

	listen(conexiones.front(), SOMAXCONN);
      }
      return 0;
    }

    /*
     * Escucha conexiones de los clientes.
     */
    int escucha() {
      sockaddr_in cliente;
      socklen_t tam_cliente = sizeof(cliente);
      this -> socket_cliente = accept(conexiones.front(), (sockaddr*)&cliente, &tam_cliente);
      char nombre_cliente[NI_MAXHOST];
      char servicio[NI_MAXSERV];

      memset(nombre_cliente, 0, NI_MAXHOST); 
      memset(servicio, 0, NI_MAXSERV);
      
      if (getnameinfo((sockaddr*)&cliente, sizeof(cliente), nombre_cliente,
		      NI_MAXHOST, servicio, NI_MAXSERV, 0) == 0){
	  cout << nombre_cliente << " connected on port " << servicio << endl;
	} else {
	  inet_ntop(AF_INET, &cliente.sin_addr, nombre_cliente, NI_MAXHOST);
        cout << nombre_cliente << " connected on port " << ntohs(cliente.sin_port) << endl;
      }
      return 0;
    }

    /*
     * Cierra socket.
     * Recibe las conexiones y el número de conexión a cerrar
     * Comenzando a contar en cero.
     */
    void cierra_socket(int num_conexion) {
      close(conexiones.front());
    }

    /*
     * Recibe mensajes y hace echo del mensaje recibido.
     */
    void sirve() {
      char buffer[4096];
      while (true)
	{
	  memset(buffer, 0, 4096);
	  
	  int bytesReceived = recv(socket_cliente, buffer, 4096, 0);
	  if (bytesReceived == -1){
	    throw runtime_error("Ocurrió un error al recibir el mensaje.");
	  }
	  
	  if (bytesReceived == 0){
	    cout << "Cliente desconectado" << endl;
	    break;
	  }
	  
	  cout << string(buffer, 0, bytesReceived) << endl;
	  send(socket_cliente, buffer, bytesReceived + 1, 0);
	}
    }

    /*
     * Cierra el socket del cliente;
     */
    void cierra_cliente() {
      close(socket_cliente);
    }
  };
}

int main() {
  servidor::Servidor servidor(1234);
  servidor.crea_conexion();
  servidor.escucha();
  servidor.cierra_socket(0);
  servidor.sirve();
  servidor.cierra_cliente();
  return 0;
}
