#pragma once

#include <gtkmm.h>

#include "ui/BasicEntry.h"

namespace RVGUI {
	class CPU;

	class Console: public Gtk::Box {
		public:
			Console();

			Console & setCPU(std::shared_ptr<CPU>);

			using Gtk::Box::append;
			void append(const Glib::ustring &, bool is_markup = false);

		private:
			std::shared_ptr<CPU> cpu;
			Gtk::ScrolledWindow scrolled;
			Gtk::TextView output;
			BasicEntry input;

			void onSubmit();
	};
}
