#ifndef CONTROLADOR_FORMA_CONEXION_H
#define CONTROLADOR_FORMA_CONEXION_H

#include "../main/cliente.hpp"
#include <gtkmm.h>
#include <string>

namespace conexion {
  Glib::RefPtr<Gtk::Application> app;
  Gtk::Window *window_ip = nullptr;
  Gtk::Box *box_ip = nullptr, *box_ip_buttons = nullptr;
  Glib::RefPtr<Gtk::Label> label_ip, label_port, label_error;
  Glib::RefPtr<Gtk::Entry> entry_ip, entry_port;
  Glib::RefPtr<Gtk::Button> button_cancel_ip, button_submit_ip;
  
  void on_app_activate(cliente::Cliente &cliente);  
}

#endif
