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
			struct VideoModeColumns: Gtk::TreeModel::ColumnRecord {
				VideoModeColumns() {
					add(name);
					add(mode);
				}

				Gtk::TreeModelColumn<Glib::ustring> name;
				Gtk::TreeModelColumn<VideoMode> mode;
			};

			sigc::signal<void(const CPU::Options &)> signal_submit_;
			Gtk::Box programBox {Gtk::Orientation::HORIZONTAL}, dataBox {Gtk::Orientation::HORIZONTAL},
			         dimensionsBox {Gtk::Orientation::HORIZONTAL}, buttonBox {Gtk::Orientation::HORIZONTAL};
			BasicEntry programFilename, dataFilename, widthEntry, heightEntry, mmioOffset, dataOffset, timeOffset,
			           memorySize;
			Gtk::CheckButton separateInstructions;
			Gtk::Button programBrowse, dataBrowse, cancelButton {"Cancel"}, clearButton {"Clear"}, okButton {"OK"};
			std::unique_ptr<Gtk::Dialog> dialog;
			Gtk::ComboBox videoModeCombo;
			Glib::RefPtr<Gtk::ListStore> videoModeModel;
			VideoModeColumns videoModeColumns;

			VideoMode videoMode = VideoMode::RGB;

			void submit();
			void clear();
			Gtk::FileChooserDialog * browse(const Glib::ustring &title, std::function<void(const std::string &)>);
			void browseProgram();
			void browseData();
			void alert(const Glib::ustring &message, Gtk::MessageType = Gtk::MessageType::INFO, bool modal = true,
			           bool use_markup = false);
			void error(const Glib::ustring &message, bool modal = true, bool use_markup = false);
			void videoModelChanged();
			void resetVideoModelCombo();
			void setDefaults();
	};
}
