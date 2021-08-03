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
	}

	HexView & HexView::setCPU(std::shared_ptr<CPU> cpu_) {
		cpu = cpu_;
		reset();
		return *this;
	}

	void HexView::reset() {
		removeChildren(grid);
	}

	bool HexView::onScroll(double, double dy) {
		adjustment->set_value(adjustment->get_value() + dy);
		return true;
	}

	void HexView::onScrolled() {
		double position = std::floor(adjustment->get_value());
		std::cout << "value[" << position << "]\n";
	}
}
