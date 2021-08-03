#pragma once

#include <gtkmm.h>

namespace RVGUI {
	class CPU;

	class HexView: public Gtk::Box {
		public:
			int rowHeight = 20, cellWidth = 30;

			HexView(int row_height = 20, int cell_width = 30);

			HexView & setCPU(std::shared_ptr<CPU>);

		private:
			std::shared_ptr<CPU> cpu;
			Gtk::Grid grid;
			Glib::RefPtr<Gtk::Adjustment> adjustment;
			Gtk::Scrollbar scrollbar;
			Glib::RefPtr<Gtk::EventControllerScroll> scrollController;

			void reset();
			bool onScroll(double dx, double dy);
			void onScrolled();
	};
}
