#include "controlador_forma_conexion.hpp"

using namespace std;

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
  
  conexion::window_ip = refBuilder -> get_widget <Gtk::Window>("window_ip");
  if (!conexion::window_ip) {
    cerr << "Could not get the window" << endl;
    return;
  }
  
  conexion::box_ip = refBuilder -> get_widget <Gtk::Box>("box_ip");
  if (!conexion::box_ip) {
    cerr << "Could not get the vetical box" << endl;
    return;
  }
  
  conexion::box_ip_buttons = refBuilder -> get_widget <Gtk::Box>("box_ip_buttons");
  if (!conexion::window_ip) {
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
    button_cancel_ip -> signal_clicked().connect([=] () {delete conexion::window_ip;});
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
	delete conexion::window_ip;
      } catch (std::invalid_argument &ia) {
	entry_port -> set_text("Puerto inválido");    
      } catch (std::runtime_error& e) {
	label_error -> set_text("No se pudo realizar la conexión"); 
      }
    });
  }
  conexion::app -> add_window(*conexion::window_ip);
  conexion::window_ip -> show();
}
