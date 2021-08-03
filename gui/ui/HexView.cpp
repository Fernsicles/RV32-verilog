#include <iomanip>
#include <iostream>
#include <sstream>

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
		scrolled.set_child(grid);
		scrolled.set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::EXTERNAL);
		append(scrolled);
		append(scrollbar);
		grid.set_hexpand(true);
		adjustment->signal_value_changed().connect(sigc::mem_fun(*this, &HexView::onScrolled));
		scrollController = Gtk::EventControllerScroll::create();
		scrollController->set_flags(Gtk::EventControllerScroll::Flags::VERTICAL);
		scrollController->signal_scroll().connect(sigc::mem_fun(*this, &HexView::onScroll), false);
		grid.add_controller(scrollController);
		sizeLabel.add_css_class("mono");
		sizeLabel.add_css_class("hidden");
		grid.attach(sizeLabel, 0, 0);
		grid.set_overflow(Gtk::Overflow::HIDDEN);
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
		widgets.clear();
		const int grid_width = grid.get_width();
		const size_t offset = static_cast<size_t>(std::floor(adjustment->get_value()));
		std::cout << "offset[" << offset << "]\n";
		const int row_count = updiv(grid.get_height(), digitHeight);
		static constexpr int GUTTER_PADDING = 3;
		const int max_row = offset + row_count - 1;
		const int digit_count = 1 + (max_row == 0? 0 : static_cast<int>(std::log2(max_row)) >> 2); // log16 = log2 / 4
		const int est_gutter_width = digit_count * (digitWidth + 1) + 2 * GUTTER_PADDING;
		if (grid_width < est_gutter_width + 2 + cellWidth)
			return;
		int cells_per_row = (grid_width - est_gutter_width - 2) / (digitWidth * 3);
		std::cout << "cells_per_row[" << cells_per_row << "], row_count[" << row_count << "], digit_count[" << digit_count << "]\n";
		if (cpu) {
			for (int row = 0; row < row_count; ++row) {
				std::stringstream ss;
				ss << std::right << std::hex << std::setw(digit_count) << std::setfill('0') << (offset + row);
				auto &label = *widgets.emplace_back(new Gtk::Label(ss.str(), Gtk::Align::END));
				label.add_css_class("mono");
				label.set_margin_start(GUTTER_PADDING);
				label.set_margin_end(GUTTER_PADDING);
				grid.attach(label, 0, row);
				auto &separator = *widgets.emplace_back(new Gtk::Separator(Gtk::Orientation::VERTICAL));
				grid.attach(separator, 1, row);
			}
		}
	}

	bool HexView::onScroll(double, double dy) {
		adjustment->set_value(adjustment->get_value() + dy);
		return true;
	}

	void HexView::onScrolled() {
		reset();
		// double position = std::floor(adjustment->get_value());
	}
}
