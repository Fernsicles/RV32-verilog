#pragma once

#include <gtkmm.h>
#include <memory>
#include <vector>

namespace RVGUI {
	class CPU;
	class MainWindow;

	class HexView: public Gtk::Box {
		public:
			int rowHeight = 20, cellWidth = 30;

			HexView(MainWindow &, int row_height = 20, int cell_width = 30);

			HexView & setCPU(std::shared_ptr<CPU>);
			void onResize();
			void update();
			void updatePC(uint32_t pc);

		private:
			struct Calculations {
				int cellsPerRow = -1, rowCount = -1, digitCount = -1, totalCells = -1;
				Calculations(): Calculations(-1, -1, -1, -1) {}
				Calculations(int cells_per_row, int row_count, int digit_count, int total_cells):
					cellsPerRow(cells_per_row), rowCount(row_count), digitCount(digit_count), totalCells(total_cells) {}
			};

			static constexpr int GUTTER_PADDING = 3;
			MainWindow &parent;
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
			std::unique_ptr<uint8_t[]> lastMemory;
			size_t lastMemorySize = 0;

			void reset();
			void updateLabel(uintptr_t cell, uint8_t value);
			std::string getLabel(uintptr_t cell);
			bool onScroll(double dx, double dy);
			void onScrolled();
			size_t getOffset() const;
			Calculations calculate(size_t offset) const;
			Calculations calculate() const;
	};
}
