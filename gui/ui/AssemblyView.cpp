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

	void AssemblyView::reset() {
		if (!cpu)
			return;

		removeChildren(grid);
		const uint64_t pc = cpu->getPC();
		for (size_t i = 0, count = cpu->getInstructionCount(); i < count; ++i) {
			std::stringstream ss;
			ss << std::hex << std::setw(4) << std::setfill('0') << std::right << i;
			grid.attach(*widgets.emplace_back(new Gtk::Label(ss.str(), Gtk::Align::END)), 0, i);
			grid.attach(*widgets.emplace_back(new Gtk::Separator(Gtk::Orientation::VERTICAL)), 1, i);
			grid.attach(*widgets.emplace_back(new Gtk::Label(disassemble(pc, cpu->getInstructions()[i]),
				Gtk::Align::START)), 2, i);
		}
	}
}
