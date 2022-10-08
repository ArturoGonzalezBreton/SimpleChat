#include "cliente.hpp"
#include "../controller/controlador_forma_conexion.hpp"
#include <iostream>
#include <gtkmm.h>
#include <string>
#include <thread>
#include <sstream>

using namespace std;

namespace {
  Glib::RefPtr<Gtk::Application> app;
  Gtk::Window *window_ip = nullptr;
  Gtk::Box *box_ip = nullptr, *box_ip_buttons = nullptr;
  Glib::RefPtr<Gtk::Label> label_ip, label_port, label_error;
  Glib::RefPtr<Gtk::Entry> entry_ip, entry_port;
  Glib::RefPtr<Gtk::Button> button_cancel_ip, button_submit_ip;
  
  void on_app_activate(cliente::Cliente &cliente) {
    
    auto refBuilder = Gtk::Builder::create();
    
    try {      
      refBuilder -> add_from_file("/home/arturogonzalezbreton/MP/chat/res/vista_forma_conexion.glade");
    } catch (const Glib::FileError& e) {
      cerr << "FileError: " << e.what() << endl;
      return;
    } catch (const Glib::MarkupError& e) {
      cerr << "MarkupError: " << e.what() << endl;
      return;
    } catch(const Gtk::BuilderError& e) {
      cerr << "BuilderError: " << e.what() << endl;
      return;
    }
    
    window_ip = refBuilder -> get_widget <Gtk::Window>("window_ip");
    if (!window_ip) {
      cerr << "Could not get the window" << endl;
      return;
    }
    
    box_ip = refBuilder -> get_widget <Gtk::Box>("box_ip");
    if (!box_ip) {
      cerr << "Could not get the vetical box" << endl;
      return;
    }
    
    box_ip_buttons = refBuilder -> get_widget <Gtk::Box>("box_ip_buttons");
    if (!window_ip) {
      cerr << "Could not get button box" << endl;
      return;
    }
    
    auto label_ip = refBuilder -> get_widget<Gtk::Label>("label_ip");
    if (!label_ip) {
      cerr << "Could not get label ip" << endl;
      return;
    }
    
    auto label_port = refBuilder -> get_widget<Gtk::Label>("label_port");
    if (!label_port) {
      cerr << "Could not get label port" << endl;
      return;
    }
    
    auto label_error = refBuilder -> get_widget<Gtk::Label>("label_error");
    if (!label_error) {
      cerr << "Could not get label error" << endl;
      return;
    }
    
    auto entry_ip = refBuilder -> get_widget<Gtk::Entry>("entry_ip");
    if (!entry_ip) {
      cerr << "Could not get entry" << endl;
      return;
    }
    
    auto entry_port = refBuilder -> get_widget<Gtk::Entry>("entry_port");
    if (!entry_port) {
      cerr << "Could not get entry port" << endl;
      return;
    }
    
    auto button_cancel_ip = refBuilder -> get_widget<Gtk::Button>("button_cancel_ip");
    if (button_cancel_ip) {
      button_cancel_ip -> signal_clicked().connect([] () {
	delete window_ip;
      });
    }
    
    auto button_submit_ip = refBuilder -> get_widget<Gtk::Button>("button_submit_ip");
    if (button_submit_ip) {
      button_submit_ip -> signal_clicked().connect([=, &cliente] () {
 	string ip = entry_ip -> get_text();
 	string puerto = entry_port -> get_text();
 	int port;
 	try {
 	  port = stoi(puerto);
 	  cliente.set_ip(ip);
 	  cliente.set_puerto(port);
 	  cliente.crea_conexion();
 	  cliente.conecta();
 	  delete window_ip;
 	} catch (std::invalid_argument &ia) {
 	  entry_port -> set_text("Puerto inválido");    
 	} catch (std::runtime_error& e) {
 	  label_error -> set_text("No se pudo realizar la conexión"); 
 	}
      });
    }
    app -> add_window(*window_ip);
    window_ip -> show();
  }
}

namespace {
  Glib ::RefPtr<Gtk::Application> app_id;
  Gtk::Window *window_id = nullptr;
  Gtk::Box *box_id = nullptr, *box_id_buttons = nullptr;
  Glib::RefPtr<Gtk::Label> label_id, id_error;
  Glib::RefPtr<Gtk::Entry> entry_id;
  Glib::RefPtr<Gtk::Button> button_cancel_id, button_submit_id;
  
  void on_id_activate(cliente::Cliente &cliente) {
    
    auto refBuilder = Gtk::Builder::create();
    
    try {      
      refBuilder -> add_from_file("/home/arturogonzalezbreton/MP/chat/res/vista_forma_id.glade");
    } catch (const Glib::FileError& e) {
      cerr << "FileError: " << e.what() << endl;
      return;
    } catch (const Glib::MarkupError& e) {
      cerr << "MarkupError: " << e.what() << endl;
      return;
    } catch(const Gtk::BuilderError& e) {
      cerr << "BuilderError: " << e.what() << endl;
      return;
    }
    
    window_id = refBuilder -> get_widget <Gtk::Window>("window_id");
    if (!window_id) {
      cerr << "Could not get the window" << endl;
      return;
    }
    
    box_id = refBuilder -> get_widget <Gtk::Box>("box_id");
    if (!box_id) {
      cerr << "Could not get the vetical box" << endl;
      return;
    }
    
    box_id_buttons = refBuilder -> get_widget <Gtk::Box>("box_id_buttons");
    if (!box_id_buttons) {
      cerr << "Could not get button box" << endl;
      return;
    }
    
    auto label_id = refBuilder -> get_widget<Gtk::Label>("label_id");
    if (!label_id) {
      cerr << "Could not get label id" << endl;
      return;
    }

    auto id_error = refBuilder -> get_widget<Gtk::Label>("id_error");
    if (!id_error) {
      cerr << "Could not get label error" << endl;
      return;
    }
    
    auto entry_id = refBuilder -> get_widget<Gtk::Entry>("entry_id");
    if (!entry_id) {
      cerr << "Could not get entry" << endl;
      return;
    }  
    
    auto button_cancel_id = refBuilder -> get_widget<Gtk::Button>("button_cancel_id");
    if (button_cancel_id) {
      button_cancel_id -> signal_clicked().connect([] () {
	delete window_id;
      });
    }
    
    auto button_submit_id = refBuilder -> get_widget<Gtk::Button>("button_submit_id");
    if (button_submit_id) {
      button_submit_id -> signal_clicked().connect([=, &cliente] () {
 	string id = entry_id -> get_text();
	string mensaje, respuesta;
	cliente.set_id(id);
	mensaje = "{ \"type\": \"IDENTIFY\", \n";
	mensaje += "\"username\" : \"";
	mensaje.append(id).append("\" }");
	try {
	  cliente.envia_mensaje(mensaje);
	  respuesta = cliente.recibe_mensajes();
	  if (respuesta.find("WARNING") != std::string::npos) 
	    id_error -> set_text("Id no disponible");
	  else
	    delete window_id;
	} catch (std::runtime_error& e) {
	  id_error -> set_text("Error en la conexión");
	}
      });
    }
    app_id -> add_window(*window_id);
    window_id -> show();
  }
}

namespace {
  Glib ::RefPtr<Gtk::Application> app_chat;
  Gtk::Window *window_chat = nullptr;
  Gtk::Grid *grid_chat = nullptr;
  Glib::RefPtr<Gtk::Label> label_status, label_user, label_room, label_message;
  Glib::RefPtr<Gtk::Entry> entry_status, entry_user, entry_room, entry_message;
  Glib::RefPtr<Gtk::Button> button_room_users, button_create_room, button_join, button_users,
    button_invite, button_leave, button_disconnect, button_send;
  
  void on_chat_activate(cliente::Cliente &cliente) {
    
    auto refBuilder = Gtk::Builder::create();
    
    try {      
      refBuilder -> add_from_file("/home/arturogonzalezbreton/MP/chat/res/vista_chat.glade");
    } catch (const Glib::FileError& e) {
      cerr << "FileError: " << e.what() << endl;
      return;
    } catch (const Glib::MarkupError& e) {
      cerr << "MarkupError: " << e.what() << endl;
      return;
    } catch(const Gtk::BuilderError& e) {
      cerr << "BuilderError: " << e.what() << endl;
      return;
    }
    
    window_chat = refBuilder -> get_widget <Gtk::Window>("window_chat");
    if (!window_chat) {
      cerr << "Could not get the window chat" << endl;
      return;
    }
    
    grid_chat = refBuilder -> get_widget <Gtk::Grid>("grid_chat");
    if (!grid_chat) {
      cerr << "Could not get grid" << endl;
      return;
    }
    
    auto label_status = refBuilder -> get_widget<Gtk::Label>("label_status");
    if (!label_status) {
      cerr << "Could not get label status" << endl;
      return;
    }

    auto label_user = refBuilder -> get_widget<Gtk::Label>("label_user");
    if (!label_user) {
      cerr << "Could not get label user" << endl;
      return;
    }
    
    auto label_room = refBuilder -> get_widget<Gtk::Label>("label_room");
    if (!label_room) {
      cerr << "Could not get label room" << endl;
      return;
    }

    auto label_message = refBuilder -> get_widget<Gtk::Label>("label_message");
    if (!label_message) {
      cerr << "Could not get label message" << endl;
      return;
    }
    
    auto entry_status = refBuilder -> get_widget<Gtk::Entry>("entry_status");
    if (!entry_status) {
      cerr << "Could not get entry status" << endl;
      return;
    }

    auto entry_user = refBuilder -> get_widget<Gtk::Entry>("entry_user");
    if (!entry_user) {
      cerr << "Could not get entry user" << endl;
      return;
    }

    auto entry_room = refBuilder -> get_widget<Gtk::Entry>("entry_room");
    if (!entry_room) {
      cerr << "Could not get entry room" << endl;
      return;
    }

    auto entry_message = refBuilder -> get_widget<Gtk::Entry>("entry_message");
    if (!entry_message) {
      cerr << "Could not get entry message" << endl;
      return;
    }  
    
    auto button_room_users = refBuilder -> get_widget<Gtk::Button>("button_room_users");
    if (button_room_users) {
      button_room_users -> signal_clicked().connect([=, &cliente] () {
	string mensaje;
	if (!(entry_room -> get_text()).empty()) {
	  mensaje = "{ \"type\": \"ROOM_USERS\", \n";
	  mensaje += "\"roomname\" : \"";
	  mensaje.append(entry_room -> get_text()).append("\" }");
	  cliente.envia_mensaje(mensaje);
	}
      });
    }

    auto button_create_room = refBuilder -> get_widget<Gtk::Button>("button_create_room");
    if (button_create_room) {
      button_create_room -> signal_clicked().connect([=, &cliente] () {
	string mensaje;
	if (!(entry_room -> get_text()).empty()) {
	  mensaje = "{ \"type\": \"NEW_ROOM\", \n";
	  mensaje += "\"roomname\" : \"";
	  mensaje.append(entry_room -> get_text()).append("\" }");
	  cliente.envia_mensaje(mensaje);
	}
      });
    }

    auto button_join = refBuilder -> get_widget<Gtk::Button>("button_join");
    if (button_join) {
      button_join -> signal_clicked().connect([=, &cliente] () {
	string mensaje;
	if (!(entry_room -> get_text()).empty()) {
	  mensaje = "{ \"type\": \"JOIN_ROOM\", \n";
	  mensaje += "\"roomname\" : \"";
	  mensaje.append(entry_room -> get_text()).append("\" }");
	  cliente.envia_mensaje(mensaje);
	}
      });
    }

    auto button_users = refBuilder -> get_widget<Gtk::Button>("button_users");
    if (button_users) {
      button_users -> signal_clicked().connect([=, &cliente] () {
	string mensaje;
	mensaje = "{ \"type\": \"USERS\" }";
	cliente.envia_mensaje(mensaje);     
      });
    }

    auto button_invite = refBuilder -> get_widget<Gtk::Button>("button_invite");
    if (button_invite) {
      button_invite -> signal_clicked().connect([=, &cliente] () {
	if (!(entry_user -> get_text()).empty() and !(entry_room -> get_text()).empty()) {
	  string mensaje;
	  string usuarios;
	  string lista = entry_user -> get_text();
	  string nombre;
	  stringstream s(lista);
	  const char separador = ' ';
	  usuarios = "[ ";
	  while (std::getline(s, nombre, separador)) {
	    usuarios += "\"";
	    usuarios.append(nombre);
	    usuarios += "\"";
	    usuarios += ",";
	  }
	  usuarios.erase(usuarios.end()-1);
	  usuarios += " ]";
	  mensaje = "{ \"type\": \"INVITE\", \n";
	  mensaje += "\"roomname\" : \"";
	  mensaje.append(entry_room -> get_text()).append("\", \n");
	  mensaje += "\"usernames\" : ";
	  mensaje.append(usuarios).append(" }");
	  cliente.envia_mensaje(mensaje);
	}
      });
    }
  
    auto button_leave = refBuilder -> get_widget<Gtk::Button>("button_leave");
  if (button_leave) {
      button_leave -> signal_clicked().connect([=, &cliente] () {
	string mensaje;
	if (!(entry_room -> get_text()).empty()) {
	  mensaje = "{ \"type\": \"LEAVE_ROOM\", \n";
	  mensaje += "\"roomname\" : \"";
	  mensaje.append(entry_room -> get_text()).append("\" }");
	  cliente.envia_mensaje(mensaje);
	}
      });
    }

    auto button_disconnect = refBuilder -> get_widget<Gtk::Button>("button_disconnect");
    if (button_disconnect) {
      button_disconnect -> signal_clicked().connect([=, &cliente] () {
	cliente.envia_mensaje("{ \"type\": \"DISCONNECT\" }\"");
	delete window_chat;
      });
    }

    auto button_send = refBuilder -> get_widget<Gtk::Button>("button_send");
    if (button_send) {
      button_send -> signal_clicked().connect([=, &cliente] () {
	string mensaje;
	if (!(entry_status -> get_text()).empty()) {
	  mensaje = "{ \"type\": \"STATUS\", \n";
	  mensaje += "\"status\" : \"";
	  mensaje.append(entry_status -> get_text()).append("\" }");
	} else if (!(entry_user -> get_text()).empty()) {
	  mensaje = "{ \"type\": \"MESSAGE\", \n";
	  mensaje += "\"username\" : \"";
	  mensaje.append(entry_user -> get_text()).append("\", \n");
	  mensaje += "\"message\" : \"";
	  mensaje.append(entry_message -> get_text()).append("\" }");
	} else if (!(entry_room -> get_text()).empty()) {
	  mensaje = "{ \"type\": \"ROOM_MESSAGE\", \n";
	  mensaje += "\"roomname\" : \"";
	  mensaje.append(entry_room -> get_text()).append("\", \n");
	  mensaje += "\"message\" : \"";
	  mensaje.append(entry_message -> get_text()).append("\" }");
	} else {
	  mensaje = "{ \"type\": \"PUBLIC_MESSAGE\", \n";
	  mensaje += "\"message\" : \"";
	  mensaje.append(entry_message -> get_text()).append("\" }");
	}
	cliente.envia_mensaje(mensaje);
      });
    }

    app_chat -> add_window(*window_chat);
    window_chat -> show();
  }
}

void impr_recv(cliente::Cliente &client);
void impr_env(cliente::Cliente &client);

void impr_recv(cliente::Cliente &client) {
  string respuesta;
  while(true) {
    try {
      respuesta = client.recibe_mensajes();
      cout << "s> " << respuesta << endl;
    } catch (std::runtime_error& e) {
      cout << e.what() << endl;
      break;
    }
  }
}

void impr_env(cliente::Cliente &client) {
  string mensaje;
  while (mensaje.find("DISCONNECT") == std::string::npos) {
    cin >> mensaje;
    client.envia_mensaje(mensaje);
  }
}



int main(int argc, char** argv) {
  cliente::Cliente cliente;

  app = Gtk::Application::create("Conexion.out");
  app -> signal_activate().connect([&] () {
    on_app_activate(cliente);
  });
  app -> run(argc, argv);

  app_id = Gtk::Application::create("Id.out");
  app_id -> signal_activate().connect([&] () {
    on_id_activate(cliente);
  });
  app_id -> run(argc, argv);

  thread hilo(impr_recv, ref(cliente));
  
  app_chat = Gtk::Application::create(cliente.get_id().append(".out"));
  app_chat -> signal_activate().connect([&] () {
    on_chat_activate(cliente);
  });
  app_chat -> run(argc, argv);
  
  hilo.join();
  cliente.desconecta();
  return 0;
}
