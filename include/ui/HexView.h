#pragma once

#include <gtkmm.h>
#include <memory>
#include <vector>

namespace RVGUI {
	class CPU;

	class HexView: public Gtk::Box {
		public:
			int rowHeight = 20, cellWidth = 30;

			HexView(int row_height = 20, int cell_width = 30);

			HexView & setCPU(std::shared_ptr<CPU>);
			void onResize();

		private:
			std::shared_ptr<CPU> cpu;
			Gtk::Grid grid;
			Glib::RefPtr<Gtk::Adjustment> adjustment;
			Gtk::ScrolledWindow scrolled;
			Gtk::Scrollbar scrollbar;
			Glib::RefPtr<Gtk::EventControllerScroll> scrollController;
			/** sizeLabel's dimensions are used for size calculations. */
			Gtk::Label sizeLabel {"0"};
			int digitWidth = -1, digitHeight = -1;
			std::vector<std::unique_ptr<Gtk::Widget>> widgets;
			std::unordered_map<int, Gtk::Label> cellLabels;
			int oldColumnCount = 0;
			int oldOffset = 0;

			void reset();
			void updateLabel(uintptr_t cell, uint8_t value);
			std::string getLabel(uintptr_t cell);
			bool onScroll(double dx, double dy);
			void onScrolled();
	};
}
