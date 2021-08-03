#pragma once

#include <gtkmm.h>

namespace RVGUI {
	class CPU;

	class HexView: public Gtk::Box {
		public:
			HexView();

			HexView & setCPU(std::shared_ptr<CPU>);

		private:
			std::shared_ptr<CPU> cpu;
			Gtk::Grid grid;
			Glib::RefPtr<Gtk::Adjustment> adjustment;
			// const Glib::RefPtr<Gtk::Adjustment> &adjustment, Gtk::Orientation orientation = Gtk::Orientation::HORIZONTAL
			Gtk::Scrollbar scrollbar;

			void onScroll();
	};
}
