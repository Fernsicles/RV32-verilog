#pragma once

#include <gtkmm.h>
#include <functional>
#include <map>
#include <vector>

#include "ui/BasicEntry.h"

namespace RVGUI {
	class CPU;

	class Console: public Gtk::Box {
		public:
			using CommandHandler = std::function<void(Console &, const std::vector<Glib::ustring> &)>;

			Console();
			std::map<Glib::ustring, CommandHandler> commands;

			Console & setCPU(std::shared_ptr<CPU>);

			using Gtk::Box::append;
			void append(const Glib::ustring &, bool is_markup = false);
			void addCommand(const Glib::ustring &, const CommandHandler &);

		private:
			std::shared_ptr<CPU> cpu;
			Gtk::ScrolledWindow scrolled;
			Gtk::TextView output;
			BasicEntry input;

			void onSubmit();
	};
}
