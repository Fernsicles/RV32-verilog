#include <iostream>

#include "ui/HexView.h"

namespace RVGUI {
	HexView::HexView():
	Gtk::Box(Gtk::Orientation::HORIZONTAL),
	adjustment(Gtk::Adjustment::create(0, 0, 100, 1, 10, 0)),
	scrollbar(adjustment, Gtk::Orientation::VERTICAL) {
		append(grid);
		append(scrollbar);
		grid.set_expand(true);
		adjustment->signal_value_changed().connect(sigc::mem_fun(*this, &HexView::onScroll));
	}

	HexView & HexView::setCPU(std::shared_ptr<CPU> cpu_) {
		cpu = cpu_;
		return *this;
	}

	void HexView::onScroll() {
		std::cout << "value[" << adjustment->get_value() << "]\n";
	}
}
