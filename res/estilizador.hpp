#ifndef ESTILIZADOR_H
#define ESTILIZADOR_H

#include <iostream>
#include <json/json.h>
#include <string>

namespace estilizador {
  
  // // Lector de cadenas.
  //  Json::Reader reader;
  // // Json a estilizar.
  //  Json::Value archivo;
  
  /*
   * Las funciones a continuación, cambian el color de la cadena
   * y dependiendo del valor del entero que reciben le dan formato.
   */
  
   std::string rojo(std::string cadena, int formato);
   std::string amarillo(std::string cadena, int formato);
   std::string verde(std::string cadena, int formato);
   std::string azul(std::string cadena, int formato);
   std::string magenta(std::string cadena, int formato);
   std::string turquesa(std::string cadena, int formato);
    
  /*
   * Da estilo a la cadena dependiendo del tipo
   * de mensaje.
   */
   std::string estiliza(std::string mensaje);
}

#endif
