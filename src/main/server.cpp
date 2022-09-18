#include "server.hpp"
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
#include <thread>

using namespace std;
using namespace cliente;
  
    
/*
 * Crea un socket y lo une al puerto en el que escuchará
 * conexiones.
 */
void servidor::Servidor::crea_conexion() {
  this -> conexion = socket(AF_INET, SOCK_STREAM, 0);      
  if (conexion == -1) {
    throw runtime_error("Ocurrió un error al crear la conexión");
  }    
  enchufe.sin_family = AF_INET;
  enchufe.sin_port = htons(this -> puerto);
  inet_pton(AF_INET, "127.0.0.1", &(enchufe.sin_addr));
  
  bind(conexion, (sockaddr*)&(enchufe), sizeof(enchufe));
  
  listen(conexion, SOMAXCONN);      
}

/*
 * Escucha conexiones de los clientes.
 */
void servidor::Servidor::escucha() {
  while (true) {
    Cliente cliente(this -> puerto);
    sockaddr_in enchufe_cliente;
    socklen_t tam_cliente = sizeof(cliente.get_socket());
    cliente.set_conexion(accept(conexion, (sockaddr*)&enchufe_cliente, &tam_cliente));
    char nombre_cliente[NI_MAXHOST];
    char servicio[NI_MAXSERV];
    
    memset(nombre_cliente, 0, NI_MAXHOST); 
    memset(servicio, 0, NI_MAXSERV);
    getnameinfo((sockaddr*)&enchufe_cliente, sizeof(cliente), nombre_cliente,
		NI_MAXHOST, servicio, NI_MAXSERV, 0);
    inet_ntop(AF_INET, &enchufe_cliente.sin_addr, nombre_cliente, NI_MAXHOST);
    
    cliente.set_socket(enchufe_cliente);
    // lanza hilo que recibe mensaje con el identificador del cliente y se lo asigna.
    clientes.insert(pair<string, Cliente>(cliente.get_id(), cliente));
    sirve(cliente);
    //thread thread(&Servidor::sirve, this, ref(cliente));
    //thread.join();
  }
}

/*
 * Asigna identificador al cliente.
 */
void servidor::Servidor::identifica_cliente(Cliente cliente, string id) {
  cliente.get_usuario().set_nombre(id);
}

/*
 * Cierra la conexion.
     */
void servidor::Servidor::cierra_socket(int num_conexion) {
  close(conexion);
}

/*
 * Recibe mensajes y decide a qué método llamar dependiendo del mensaje recibido.
 */
void servidor::Servidor::sirve(Cliente cliente) {
  char buffer[4096];
  while (true)
    {
      memset(buffer, 0, 4096);
      
      int msj_recibido = recv(cliente.get_conexion(), buffer, 4096, 0);
      if (msj_recibido == -1){
	throw runtime_error("Ocurrió un error al recibir el mensaje.");
      }
      
      if (msj_recibido == 0){
	cout << "Cliente desconectado" << endl;
	break;
      }
      
      cout << string(buffer, 0, msj_recibido) << endl;
      send(cliente.get_conexion(), buffer, msj_recibido + 1, 0);
    }
}

/*
 * Cierra la conexion del cliente;
 */
void servidor::Servidor::cierra_cliente(string id) {
  close(clientes[id].get_conexion());
}
