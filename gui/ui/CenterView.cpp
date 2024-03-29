#include <iostream>

#include "ui/CenterView.h"

namespace RVGUI {
	CenterView::CenterView(Gtk::Widget *child_): CenterView(child_, 0, 0) {}

	CenterView::CenterView(Gtk::Widget *child_, int width_, int height_):
	Gtk::Fixed(), child(child_), width(width_), height(height_) {
		if (child)
			put(*child, 0, 0);
		add_tick_callback([this](const auto &) {
			int new_width = get_width(), new_height = get_height();
			if (new_width != lastWidth || new_height != lastHeight) {
				lastWidth = new_width;
				lastHeight = new_height;
				updateChild();
			}
			return true;
		});
	}

	void CenterView::setDimensions(int width_, int height_) {
		width = width_;
		height = height_;
		updateChild();
	}

	void CenterView::setChild(Gtk::Widget &new_child) {
		if (&new_child == child)
			return;
		if (child)
			remove(*child);
		child = &new_child;
		put(*child, 0, 0);
		updateChild();
	}

	void CenterView::updateChild() {
		if (child) {
			move(*child, (lastWidth - width) / 2, (lastHeight - height) / 2);
			child->set_size_request(width, height);
		}
	}
}
