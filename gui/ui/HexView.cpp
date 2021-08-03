#include <iostream>

#include "Util.h"
#include "ui/HexView.h"
#include "ui/Util.h"

namespace RVGUI {
	HexView::HexView(int row_height, int cell_width):
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	rowHeight(row_height),
	cellWidth(cell_width),
	adjustment(Gtk::Adjustment::create(0, 0, 100, 1, 10, 0)),
	scrollbar(adjustment, Gtk::Orientation::VERTICAL) {
		set_overflow(Gtk::Overflow::HIDDEN);
		append(grid);
		append(scrollbar);
		grid.set_expand(true);
		adjustment->signal_value_changed().connect(sigc::mem_fun(*this, &HexView::onScrolled));
		scrollController = Gtk::EventControllerScroll::create();
		scrollController->set_flags(Gtk::EventControllerScroll::Flags::VERTICAL);
		scrollController->signal_scroll().connect(sigc::mem_fun(*this, &HexView::onScroll), false);
		grid.add_controller(scrollController);
		sizeLabel.add_css_class("mono");
		sizeLabel.add_css_class("hidden");
		grid.attach(sizeLabel, 0, 0);
	}

	HexView & HexView::setCPU(std::shared_ptr<CPU> cpu_) {
		cpu = cpu_;
		reset();
		return *this;
	}

	void HexView::onResize() {
		reset();
	}

	void HexView::reset() {
		if (digitWidth == -1) {
			digitWidth  = sizeLabel.get_width();
			digitHeight = sizeLabel.get_height();
			if (digitWidth == 0) {
				digitWidth = -1;
				return;
			}
		}

		removeChildren(grid);
		const int grid_width = grid.get_width();
		const double offset = std::floor(adjustment->get_value());
		const int row_count = updiv(grid.get_height(), digitHeight);
		const int est_gutter_width = (1 + (offset == 0? 0 : std::log10(offset + row_count - 1))) * (digitWidth + 1);
		if (grid_width < est_gutter_width + 2 + cellWidth)
			return;
		int cells_per_row = (grid_width - est_gutter_width - 2) / (digitWidth * 3);
		std::cout << "cells_per_row[" << cells_per_row << "], row_count[" << row_count << "]\n";
		if (cpu) {

		}
	}

	bool HexView::onScroll(double, double dy) {
		adjustment->set_value(adjustment->get_value() + dy);
		return true;
	}

	void HexView::onScrolled() {
		// double position = std::floor(adjustment->get_value());
	}
}
