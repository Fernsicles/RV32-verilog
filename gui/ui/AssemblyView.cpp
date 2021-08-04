#include <iomanip>
#include <sstream>

#include "CPU.h"
#include "lib/Disassembler.h"
#include "ui/AssemblyView.h"
#include "ui/Util.h"

namespace RVGUI {
	AssemblyView::AssemblyView(): Gtk::ScrolledWindow() {
		set_child(grid);
		add_css_class("assemblyview");
	}

	void AssemblyView::setCPU(std::shared_ptr<CPU> cpu_) {
		cpu = cpu_;
		reset();
	}

	void AssemblyView::updatePC() {
		if (activeGutter)
			activeGutter->remove_css_class("pc");
		if (activeDisassembled)
			activeDisassembled->remove_css_class("pc");
		const auto pc = cpu->getPC();
		if (labels.count(pc) != 0) {
			auto &gutter_label = labels.at(pc);
			auto &disassembled = labels.at(pc + 1);
			gutter_label.add_css_class("pc");
			disassembled.add_css_class("pc");
			activeGutter = &gutter_label;
			activeDisassembled = &disassembled;
		}
	}

	void AssemblyView::reset() {
		if (!cpu)
			return;

		removeChildren(grid);
		widgets.clear();
		labels.clear();
		const uint64_t pc = cpu->getPC();
		for (size_t i = 0, count = cpu->getInstructionCount(); i < count; ++i) {
			std::stringstream ss;
			ss << std::hex << std::setw(4) << std::setfill('0') << std::right << 4 * i;
			auto &gutter_label = labels.try_emplace(4 * i, ss.str(), Gtk::Align::END).first->second;
			auto &disassembled = labels.try_emplace(4 * i + 1, disassemble(pc, cpu->getInstructions()[i]),
				Gtk::Align::START).first->second;
			grid.attach(gutter_label, 0, i);
			grid.attach(*widgets.emplace_back(new Gtk::Separator(Gtk::Orientation::VERTICAL)), 1, i);
			grid.attach(disassembled, 2, i);
			if (pc == 4 * i) {
				gutter_label.add_css_class("pc");
				disassembled.add_css_class("pc");
				activeGutter = &gutter_label;
				activeDisassembled = &disassembled;
			}
		}
	}
}
