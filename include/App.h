#pragma once

#include <gtkmm.h>

#include "ui/MainWindow.h"

namespace RVGUI {
	class App: public Gtk::Application {
		public:
			static Glib::RefPtr<App> create();

			void on_startup() override;
			void on_activate() override;

			MainWindow * create_window();

		protected:
			App();

		private:
			void on_hide_window(Gtk::Window *);	
	};
}
