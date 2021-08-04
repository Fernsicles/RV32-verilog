#pragma once

#include <gtkmm.h>
#include <memory>
#include <unordered_map>
#include <vector>

namespace RVGUI {
	class CPU;
	class MainWindow;

	class AssemblyView: public Gtk::ScrolledWindow {
		public:
			AssemblyView(MainWindow &);

			void setCPU(std::shared_ptr<CPU>);
			void updatePC(uint32_t pc);

		private:
			MainWindow &parent;
			Gtk::Grid grid;
			std::shared_ptr<CPU> cpu;
			std::vector<std::unique_ptr<Gtk::Widget>> widgets;
			std::vector<Glib::RefPtr<Gtk::GestureClick>> gestures;
			std::unordered_map<uint64_t, Gtk::Label> labels;
			Gtk::Label *activeGutter = nullptr, *activeDisassembled = nullptr;

			void reset();
	};
}
