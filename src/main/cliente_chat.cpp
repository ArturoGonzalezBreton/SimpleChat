#include "cliente.hpp"
#include <iostream>
#include <gtkmm.h>
#include <string>
#include <thread>

using namespace std;

namespace {
  Glib ::RefPtr<Gtk::Application> app;
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
      button_cancel_ip -> signal_clicked().connect([] () {delete window_ip;});
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
  string id;
  string ip;
  string puerto;
  cliente::Cliente cliente;
  
  app = Gtk::Application::create("Chat.out");
  app -> signal_activate().connect([&] () {on_app_activate(cliente);});
  app -> run(argc, argv);

  cout << cliente.get_ip() << endl;
  cout << to_string(cliente.get_puerto()) << endl;
  
  string mensaje;
  string respuesta;

  if (cliente.get_id().empty()) {
    do {
      cout << "Ingrese un identificador" << endl;
      cin >> id;
      cliente.set_id(id);
      mensaje = "{ \"type\": \"IDENTIFY\", \n";
      mensaje += "\"username\" : \"";
      mensaje.append(id).append("\" }");
      cliente.envia_mensaje(mensaje);
      respuesta = cliente.recibe_mensajes();
      cout << "s> " << respuesta << endl;
    } while (respuesta.find("WARNING") != std::string::npos);
  }
  thread hilo(impr_env, ref(cliente));
  impr_recv(cliente);
  hilo.join();
  cliente.desconecta();
  return 0;
}
