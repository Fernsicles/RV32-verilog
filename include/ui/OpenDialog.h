#pragma once

#include <gtkmm.h>
#include <memory>

#include "CPU.h"
#include "ui/BasicEntry.h"

namespace RVGUI {
	class OpenDialog: public Gtk::Dialog {
		public:
			OpenDialog(Gtk::Window &parent, bool modal = true);

			sigc::signal<void(const CPU::Options &)> signal_submit() const { return signal_submit_; }

		private:
			sigc::signal<void(const CPU::Options &)> signal_submit_;
			Gtk::Box programBox {Gtk::Orientation::HORIZONTAL}, dataBox {Gtk::Orientation::HORIZONTAL},
			         dimensionsBox {Gtk::Orientation::HORIZONTAL}, buttonBox {Gtk::Orientation::HORIZONTAL};
			BasicEntry programFilename, dataFilename, width, height, mmioOffset, dataOffset, timeOffset, memorySize;
			Gtk::CheckButton separateInstructions;
			Gtk::Button programBrowse, dataBrowse, cancelButton {"Cancel"}, clearButton {"Clear"}, okButton {"OK"};
			std::unique_ptr<Gtk::Dialog> dialog;

			void clear();
			Gtk::FileChooserDialog * browse(const Glib::ustring &title, std::function<void(const std::string &)>);
			void browseProgram();
			void browseData();
	};
}
