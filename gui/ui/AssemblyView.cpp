#include <iomanip>
#include <sstream>

#include "CPU.h"
#include "lib/Disassembler.h"
#include "ui/AssemblyView.h"
#include "ui/MainWindow.h"
#include "ui/Util.h"

namespace RVGUI {
	AssemblyView::AssemblyView(MainWindow &parent_): Gtk::ScrolledWindow(), parent(parent_) {
		set_child(grid);
		add_css_class("assemblyview");
	}

	void AssemblyView::setCPU(std::shared_ptr<CPU> cpu_) {
		cpu = cpu_;
		reset();
	}

	void AssemblyView::updatePC(uint32_t pc) {
		if (activeGutter)
			activeGutter->remove_css_class("pc");
		if (activeDisassembled)
			activeDisassembled->remove_css_class("pc");
		if (labels.count(pc) != 0) {
			auto &gutter_label = labels.at(pc);
			auto &disassembled = labels.at(pc + 1);
			gutter_label.add_css_class("pc");
			disassembled.add_css_class("pc");
			activeGutter = &gutter_label;
			activeDisassembled = &disassembled;
			auto adjustment = get_vadjustment();
			const int label_height = gutter_label.get_height();
			const unsigned offset = pc * label_height / 4;
			if (offset < adjustment->get_value() || adjustment->get_value() + adjustment->get_page_size() < offset)
				adjustment->set_value(offset);
		}
	}

	void AssemblyView::reset() {
		removeChildren(grid);
		widgets.clear();
		labels.clear();
		gestures.clear();

		if (!cpu)
			return;

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
			auto &gutter_gesture = gestures.emplace_back(Gtk::GestureClick::create());
			gutter_gesture->signal_released().connect([this, i](int, double, double) {
				cpu->setPC(4 * i);
				parent.onUpdatePC(4 * i);
			});
			auto &disassembled_gesture = gestures.emplace_back(Gtk::GestureClick::create());
			disassembled_gesture->signal_released().connect([this, i](int, double, double) {
				cpu->setPC(4 * i);
				parent.onUpdatePC(4 * i);
			});
			gutter_label.add_controller(gutter_gesture);
			disassembled.add_controller(disassembled_gesture);
			if (pc == 4 * i) {
				gutter_label.add_css_class("pc");
				disassembled.add_css_class("pc");
				activeGutter = &gutter_label;
				activeDisassembled = &disassembled;
			}
		}
	}
}
