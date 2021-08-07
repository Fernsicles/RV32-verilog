#include "CPU.h"
#include "ui/RegisterView.h"

namespace RVGUI {
	RegisterView::RegisterView(): Gtk::ScrolledWindow() {
		add_css_class("registerview");
		nameLabels.reserve(COUNT);
		valueLabels.reserve(COUNT);
		separators.reserve(COUNT);
		lastValues.reserve(COUNT);
		for (size_t i = 0; i < COUNT; ++i) {
			grid.attach(nameLabels.emplace_back(names[i]), 0, i);
			grid.attach(separators.emplace_back(), 1, i);
			grid.attach(valueLabels.emplace_back("-", Gtk::Align::START), 2, i);
			lastValues.push_back(0xdeadbeef);
		}
		set_child(grid);
	}

	void RegisterView::setCPU(std::shared_ptr<CPU> cpu_) {
		cpu = cpu_;
	}

	void RegisterView::update() {
		if (!cpu)
			return;
		for (size_t i = 0; i < COUNT; ++i) {
			const Word new_value = cpu->getRegister(i);
			if (new_value != lastValues[i]) {
				lastValues[i] = new_value;
				valueLabels[i].set_text(std::to_string(new_value));
			}
		}		
	}
}
