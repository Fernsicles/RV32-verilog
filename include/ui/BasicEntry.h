#pragma once

#include <gtkmm.h>

namespace RVGUI {
	class BasicEntry: public Gtk::Entry {
		public:
			BasicEntry();
			sigc::signal<void()> signal_activate() const { return signal_activate_; }
			bool get_focused() const { return focused; }

		private:
			bool focused = false;
			Glib::RefPtr<Gtk::EventControllerFocus> focusController;
			sigc::signal<void()> signal_activate_;
	};
}
