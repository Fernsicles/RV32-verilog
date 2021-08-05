#include <iostream>

#include "App.h"

namespace RVGUI {
	App::App(): Gtk::Application("com.ferns.rvgui", Gio::Application::Flags::NONE) {}

	Glib::RefPtr<App> App::create() {
		return Glib::make_refptr_for_instance<App>(new App());
	}

	void App::on_startup() {
		Gtk::Application::on_startup();
		set_accel_for_action("win.open", "<Ctrl>o");
		set_accel_for_action("win.play", "<Ctrl>p");
	}

	void App::on_activate() {
		try {
			create_window()->present();
		} catch (const Glib::Error &err) {
			std::cerr << "App::on_activate(): Glib::Error: " << err.what() << std::endl;
		} catch (const std::exception &err) {
			std::cerr << "App::on_activate(): " << err.what() << std::endl;
		}
	}

	MainWindow * App::create_window() {
		MainWindow *window = MainWindow::create();
		add_window(*window);
		window->signal_hide().connect(sigc::bind(sigc::mem_fun(*this, &App::on_hide_window), window));
		return window;
	}

	void App::on_hide_window(Gtk::Window *window) {
		delete window;
	}
}
